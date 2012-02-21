#include "MMBNBattleActor.h"
#include "ImgManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

map<MMBNBattleActor::ActorState, string> 	MMBNBattleActor::FOLDER_NAMES_OF_STATES			;
map<MMBNBattleActor::ActorState, bool>		MMBNBattleActor::LOOP_OF_STATES					;

void MMBNBattleActor::Initialize()
{
	FOLDER_NAMES_OF_STATES[BATTLE_STANDING] 	= "/Battle/battle_standing"		;
	FOLDER_NAMES_OF_STATES[BATTLE_DAMAGED] 		= "/Battle/battle_damaged"		;
	FOLDER_NAMES_OF_STATES[BATTLE_DEAD] 		= "/Battle/battle_dead"			;
	FOLDER_NAMES_OF_STATES[BATTLE_ATTACK] 		= "/Battle/battle_attack"		;
	FOLDER_NAMES_OF_STATES[BATTLE_WIN]	 		= "/Battle/battle_win"			;
	
	LOOP_OF_STATES[BATTLE_STANDING]				= true							;
	LOOP_OF_STATES[BATTLE_DAMAGED]				= false							;
	LOOP_OF_STATES[BATTLE_DEAD]					= false							;
	LOOP_OF_STATES[BATTLE_ATTACK]				= false							;
	LOOP_OF_STATES[BATTLE_WIN]					= false							;
}



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
MMBNBattleActor::MMBNBattleActor()
{
	m_posX = 0;
	m_posY = 0;
	m_offX = 0;
	m_offY = 0;
}

MMBNBattleActor::MMBNBattleActor(string name, bool loadNormalSprites, bool loadReverseSprites)
{
	m_name = name; // /!\ à mettre au début car les autres fonctions en ont besoin
	m_load_normal_sprites = loadNormalSprites;
	m_load_reversed_sprites = loadReverseSprites;
	
	//===============================
	// ANIMATIONS
	//===============================
	// BATTLE
	//===============================
	LoadStateAnim(BATTLE_STANDING);
	LoadStateAnim(BATTLE_ATTACK);
	LoadStateAnim(BATTLE_DAMAGED);
	LoadStateAnim(BATTLE_DEAD);
	LoadStateAnim(BATTLE_WIN);
	
	//-------------------------------

	m_direction = RIGHT;
	m_state = BATTLE_STANDING;

	m_lv	= 1							;
	m_hp	= 100						;
	m_maxHp	= 100						;
	m_atk	= 10							;
	m_chrg	= 1							;
	m_spd	= 1							;

	m_posX = 0;
	m_posY = 0;
	
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

MMBNBattleActor::~MMBNBattleActor()
{

	//=========================================
	// SUPPRESSION DES ANIMATIONS
	//=========================================
	/*std::map<ActorState,Animation*>::iterator it;
	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).second ) delete (*it).second;

	for(it = m_reversedAnimMap.begin() ; it != m_reversedAnimMap.end() ; ++it)
		if( (*it).second ) delete (*it).second;*/

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
MMBNBattleActor* MMBNBattleActor::Load(std::string name, bool ia, bool loadNormalSprites, bool loadReverseSprites)
{
	#ifdef _DEBUG
	LOG("Load actor " + name)
	#endif
	MMBNBattleActor* a = new MMBNBattleActor(name, loadNormalSprites, loadReverseSprites);

	a->m_is_ia = ia;
	
	a->InitializeInfo();
	
	if(ia) a->InitializeIA();
	
	#ifdef _DEBUG
	LOG("Actor loaded")
	#endif
	
	return a;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE IA
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleActor::InitializeIA()
{
	ifstream in_ia( (string("Actors/") + m_name + string("/ia.txt")).c_str() , ifstream::in);

	if(!in_ia.good())
	{
		LOG("Impossible de trouver le fichier ia.txt pour " + string("Actors/") + m_name);
		oslQuit();
	}

	string line;
	while(getline(in_ia, line))
	{
		if(line.find("moving_time") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->m_ia_config.moving_time;
		}
		else if(line.find("attack_time") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->m_ia_config.attack_time;
		}
		else if(line.find("wait_after_attack_time") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->m_ia_config.wait_after_attack_time;
		}
	}

	in_ia.close();
	
	this->m_ia_config.current_attack = -1;
	
	InitializeIASkills();
}

void MMBNBattleActor::InitializeIASkills()
{
	ifstream in_skills( (string("Actors/") + m_name + string("/Battle/IA/Skills/skills.txt")).c_str() , ifstream::in);

	if(!in_skills.good())
	{
		LOG("Impossible de trouver le fichier skills.txt pour " + string("Actors/") + m_name + string("/Battle/IA/Skills/skills.txt"));
		oslQuit();
	}

	string line;
	while(getline(in_skills, line))
	{
		vector<string> v = StringUtils::Split(line, " \r\n");
		GameSystem::BattleAttack ba;
		
		GameSystem::InitAttackInfo( string("Actors/") + m_name + string("/Battle/IA/Skills/") + v.at(0) + string("/attack_info.txt"), &(ba.attack_info) );
		GameSystem::InitBattleAttack( string("Actors/") + m_name + string("/Battle/IA/Skills/") + v.at(0) + string("/") + v.at(0) + string(".txt"), &ba );
		ba.actor_animation->Reverse();
		
		this->m_ia_config.battle_attacks.push_back(ba);
	}

	in_skills.close();
	
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE INFO
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleActor::InitializeInfo()
{
	ifstream in_info( (string("Actors/") + m_name + string("/info.txt")).c_str() , ifstream::in);

	if(!in_info.good())
	{
		LOG("Impossible de trouver le fichier info.txt pour " + string("Actors/") + m_name);
		oslQuit();
	}

	string line;
	while(getline(in_info, line))
	{
		//ATTACK
		//frame
		if(line.find("attack_frame") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->m_actor_info.attack_frame;
		}
		//range
		else if(line.find("attack_range") == 0)
		{
			vector<Vector2i> range;
			
			vector<string> v = StringUtils::Split(line, " \r\n");
			for(unsigned int i = 1 ; i < v.size() ; ++i)
			{
				vector<string> r = StringUtils::Split(v[i], ",");
				istringstream iss(r.at(0) + " " + r.at(1));
				int x_range;
				int y_range;
				iss >> x_range;
				iss >> y_range;
				range.push_back(Vector2i(x_range,y_range));
			}
			
			this->m_actor_info.attack_info.range = range;
		}
		//power
		else if(line.find("attack_power") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			istringstream iss(v.at(1));
			iss >> this->m_atk;
		}
		//stagger enemy
		else if(line.find("attack_stagger") == 0)
		{
			vector<string> v = StringUtils::Split(line, " \r\n");
			if(v.at(1).compare("true") == 0)
				this->m_actor_info.attack_info.stagger_enemy = true;
			else
				this->m_actor_info.attack_info.stagger_enemy = false;
		}
	}

	in_info.close();
	
	this->m_actor_info.attack_info.target_type = GameSystem::USER_TARGET;
	
	vector<GameSystem::PanelTeam> teams;
	teams.push_back(GameSystem::PLAYER);
	teams.push_back(GameSystem::ENEMY);
	this->m_actor_info.attack_info.target_teams = teams;
	
	this->m_actor_info.attack_info.pierce_attack = true;
	//GameSystem::AttackInfo info = { GameSystem::USER_TARGET, range, teams, true};
	
	//this->m_actor_info.attack_info = info;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleActor::LoadStateAnim(ActorState state)
{
	//Animation *anim = NULL, *rAnim = NULL;
	
	if(m_load_normal_sprites)
	{
		AnimationPtr anim = Animation::Load(string("Actors/") + m_name + FOLDER_NAMES_OF_STATES[state], false, LOOP_OF_STATES[state]);
		m_animMap.insert(pair<ActorState, AnimationPtr>(state, anim));
	}
	if(m_load_reversed_sprites)
	{
		AnimationPtr rAnim = Animation::Load(string("Actors/") + m_name + FOLDER_NAMES_OF_STATES[state], true, LOOP_OF_STATES[state]);
		m_reversedAnimMap.insert(pair<ActorState, AnimationPtr>(state, rAnim));
	}
	
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleActor::Display(float offX, float offY)
{

	m_offX = offX;
	m_offY = offY;

	//is using skill
	if(m_state == BATTLE_SKILL)
	{
		//is ia
		if(m_is_ia)
		{
			if( (m_ia_config.current_attack >= 0) && (m_ia_config.current_attack < m_ia_config.battle_attacks.size()) )
			{
				m_ia_config.battle_attacks[m_ia_config.current_attack].actor_animation->Update();
				m_ia_config.battle_attacks[m_ia_config.current_attack].actor_animation->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
				m_ia_config.battle_attacks[m_ia_config.current_attack].actor_animation->GetCurrentSprite().Display();
				#ifdef _DEBUG
					m_ia_config.battle_attacks[m_ia_config.current_attack].actor_animation->GetCurrentSprite().DisplayExtension();
				#endif
			}
		}
		return;
	}
	
	//if only the normal sprites have been loaded, we don't care about direction
	if(m_load_normal_sprites && !m_load_reversed_sprites)
	{
		m_animMap[m_state]->Update();
		m_animMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
		m_animMap[m_state]->GetCurrentSprite().Display();
		#ifdef _DEBUG
			m_animMap[m_state]->GetCurrentSprite().DisplayExtension();
		#endif
	}
	//if only the reversed sprites have been loaded, we don't care about direction
	else if(!m_load_normal_sprites && m_load_reversed_sprites)
	{
		m_reversedAnimMap[m_state]->Update();
		m_reversedAnimMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
		m_reversedAnimMap[m_state]->GetCurrentSprite().Display();
		#ifdef _DEBUG
			m_reversedAnimMap[m_state]->GetCurrentSprite().DisplayExtension();
		#endif
	}
	//if both normal and reversed sprites have been loaded, look the direction to choose the correct sprite to display
	else
	{
		if(m_direction == RIGHT)
		{
			m_animMap[m_state]->Update();
			m_animMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
			m_animMap[m_state]->GetCurrentSprite().Display();
			#ifdef _DEBUG
				m_animMap[m_state]->GetCurrentSprite().DisplayExtension();
			#endif
		}
		else
		{
			m_reversedAnimMap[m_state]->Update();
			m_reversedAnimMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
			m_reversedAnimMap[m_state]->GetCurrentSprite().Display();
			#ifdef _DEBUG
				m_reversedAnimMap[m_state]->GetCurrentSprite().DisplayExtension();
			#endif
		}

	}

}

void MMBNBattleActor::Move(float x, float y)
{
	this->SetPosition(m_posX + x, m_posY + y);
}

void MMBNBattleActor::SetDirection(ActorDirection direction)
{
	if(m_direction == direction) return;

	if(direction == LEFT) m_direction = LEFT;
	else if(direction == RIGHT) m_direction = RIGHT;
}

void MMBNBattleActor::SetState(ActorState state)
{
	if(m_state == state) return;

	//===============================
	// BATTLE
	//===============================
	/*if(state == BATTLE_STANDING) m_state = BATTLE_STANDING;
	else if(state == BATTLE_ATTACK) m_state = BATTLE_ATTACK;
	else if(state == BATTLE_DAMAGED) m_state = BATTLE_DAMAGED;
	else if(state == BATTLE_DEAD) m_state = BATTLE_DEAD;*/
	
	m_state = state;
	
	if(m_state == BATTLE_SKILL)
	{
		for(unsigned int i = 0 ; m_is_ia && (i < m_ia_config.battle_attacks.size()) ; ++i)
			m_ia_config.battle_attacks[i].actor_animation->Stop();
	}
	else
	{
		//stop current anim
		if(m_load_normal_sprites) m_animMap[m_state]->Stop();
		if(m_load_reversed_sprites) m_reversedAnimMap[m_state]->Stop();
	}
	
	//===================================
	// ON STOPPE TOUTES LES ANIMATIONS SAUF CELLE DE L'ETAT COURANT
	//===================================
	/*std::map<ActorState,Animation*>::iterator it;

	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).first != state ) m_animMap[(*it).first]->Stop();

	for(it = m_reversedAnimMap.begin() ; it != m_reversedAnimMap.end() ; ++it)
		if( (*it).first != state ) m_reversedAnimMap[(*it).first]->Stop();*/
}

void MMBNBattleActor::SetPosition(float x, float y)
{
	m_posX = x;
	m_posY = y;
}

MMBNBattleActor::ActorDirection MMBNBattleActor::GetDirection() const
{
	return m_direction;
}

MMBNBattleActor::ActorState MMBNBattleActor::GetState() const
{
	return m_state;
}

string MMBNBattleActor::GetName() const
{
	return m_name;
}

AnimationPtr MMBNBattleActor::GetCurrentAnim()
{
	if(m_direction == RIGHT)
		return m_animMap[m_state];
	else
		return m_reversedAnimMap[m_state];
}

Vector2f& MMBNBattleActor::GetPosition()
{
	m_position.x = m_posX;
	m_position.y = m_posY;
	return m_position;
}

int MMBNBattleActor::GetLV() const
{
	return m_lv;
}

int MMBNBattleActor::GetHP() const
{
	return m_hp;
}

int MMBNBattleActor::GetMaxHP() const
{
	return m_maxHp;
}
int MMBNBattleActor::GetAtk() const
{
	return m_atk;
}

int MMBNBattleActor::GetChrg() const
{
	return m_chrg;
}

int MMBNBattleActor::GetSpd() const
{
	return m_spd;
}

MMBNBattleActor::IAConfig* MMBNBattleActor::GetIAConfig()
{
	return &m_ia_config;
}

MMBNBattleActor::BattleActorInfo* MMBNBattleActor::GetInfo()
{
	return &m_actor_info;
}

void MMBNBattleActor::Attack(MMBNBattleActor* mmbnba)
{
	mmbnba->m_hp -= this->m_atk;
	if(mmbnba->m_hp < 0) mmbnba->m_hp = 0;
}

void MMBNBattleActor::SkillAttack(MMBNBattleActor* target, GameSystem::BattleAttack* skill)
{
	target->m_hp -= skill->power;
	if(target->m_hp < 0) target->m_hp = 0;
}

bool MMBNBattleActor::IsDead()
{
	return (m_hp <= 0);
}
