#include "MMBNBattleActor.h"
#include "ImgManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

map<MMBNBattleActor::ActorState, string> MMBNBattleActor::FOLDER_NAMES_OF_STATES		;
map<MMBNBattleActor::ActorState, bool>	MMBNBattleActor::LOOP_OF_STATES				;

void MMBNBattleActor::Initialize()
{
	FOLDER_NAMES_OF_STATES[BATTLE_STANDING] 	= "/Battle/battle_standing"		;
	FOLDER_NAMES_OF_STATES[BATTLE_DAMAGED] 		= "/Battle/battle_damaged"		;
	FOLDER_NAMES_OF_STATES[BATTLE_DEAD] 		= "/Battle/battle_dead"			;
	FOLDER_NAMES_OF_STATES[BATTLE_ATTACK] 		= "/Battle/battle_attack"		;
	
	LOOP_OF_STATES[BATTLE_STANDING]				= true							;
	LOOP_OF_STATES[BATTLE_DAMAGED]				= false							;
	LOOP_OF_STATES[BATTLE_DEAD]					= false							;
	LOOP_OF_STATES[BATTLE_ATTACK]				= false							;
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
	MMBNBattleActor* a = new MMBNBattleActor(name, loadNormalSprites, loadReverseSprites);

	if(ia) a->InitializeIA();

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
			vector<string> v = StringUtils::Split(line, " ");
			istringstream iss(v.at(1));
			iss >> this->m_ia_config.moving_time;
		}
	}

	in_ia.close();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INIT DELAYS OF ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/*void MMBNBattleActor::InitDelaysOfAnim(string name, vector<int> &delays)
{
	int spritesNb = 0, delay = 0;
	ifstream in_anim( (string("Actors/") + m_name + string("/") + name + string("/anim.txt")).c_str() , ifstream::in);

	if(!in_anim.good())
	{
		LOG("Impossible de trouver le fichier anim.txt pour " + string("Actors/") + m_name + string("/") + name);
		oslQuit();
	}

	string line;
	while(getline(in_anim, line))
	{
		if(line.find("sprites=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			istringstream iss(line);
			iss >> spritesNb;
		}
		else if(line.find("delays=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			int commaIndex = -1;

			for(int i = 0 ; i < spritesNb ; i++)
			{
				commaIndex = line.find_first_of(",");

				string delay_s;
				if(commaIndex != -1) delay_s = line.substr(0, commaIndex);
				else delay_s = line;

				istringstream iss(delay_s);
				iss >> delay;
				delays.push_back(delay);

				line = line.substr(commaIndex + 1);
			}
			
		}
	}

	in_anim.close();
}*/

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
	if(state == BATTLE_STANDING) m_state = BATTLE_STANDING;
	else if(state == BATTLE_ATTACK) m_state = BATTLE_ATTACK;
	else if(state == BATTLE_DAMAGED) m_state = BATTLE_DAMAGED;
	else if(state == BATTLE_DEAD) m_state = BATTLE_DEAD;
	
	//stop current anim
	if(m_load_normal_sprites) m_animMap[m_state]->Stop();
	if(m_load_reversed_sprites) m_reversedAnimMap[m_state]->Stop();

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

void MMBNBattleActor::Attack(MMBNBattleActor* mmbnba)
{
	mmbnba->m_hp -= this->m_atk;
	if(mmbnba->m_hp < 0) mmbnba->m_hp = 0;
}

bool MMBNBattleActor::IsDead()
{
	return (m_hp <= 0);
}
