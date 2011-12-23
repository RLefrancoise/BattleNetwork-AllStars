#include "MMBNBattleActor.h"
#include "ImgManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

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

MMBNBattleActor::MMBNBattleActor(string name)
{
	m_name = name; // /!\ à mettre au début car les autres fonctions en ont besoin

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
	std::map<ActorState,Animation*>::iterator it;
	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).second ) delete (*it).second;

	for(it = m_reversedAnimMap.begin() ; it != m_reversedAnimMap.end() ; ++it)
		if( (*it).second ) delete (*it).second;

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
MMBNBattleActor* MMBNBattleActor::Load(std::string name, bool ia)
{
	MMBNBattleActor* a = new MMBNBattleActor(name);

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
void MMBNBattleActor::InitDelaysOfAnim(string name, vector<int> &delays)
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
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNBattleActor::LoadStateAnim(ActorState state)
{
	Animation *anim = NULL, *rAnim = NULL;
	std::vector<int> delays;

	switch(state)
	{
		//===============================
		// BATTLE
		//===============================
		case BATTLE_STANDING:
			InitDelaysOfAnim("Battle/battle_standing", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_standing"), delays, false);
			rAnim = new Animation(string("Actors/") + m_name + string("/Battle/battle_standing"), delays, true);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_STANDING, anim));
			m_reversedAnimMap.insert(pair<ActorState, Animation*>(BATTLE_STANDING, rAnim));
			break;

		case BATTLE_ATTACK:
			InitDelaysOfAnim("Battle/battle_attack", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_attack"), delays, false, false);
			rAnim = new Animation(string("Actors/") + m_name + string("/Battle/battle_attack"), delays, true, false);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_ATTACK, anim));
			m_reversedAnimMap.insert(pair<ActorState, Animation*>(BATTLE_ATTACK, rAnim));
			break;

		case BATTLE_DAMAGED:
			InitDelaysOfAnim("Battle/battle_damaged", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_damaged"), delays, false, false);
			rAnim = new Animation(string("Actors/") + m_name + string("/Battle/battle_damaged"), delays, true, false);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_DAMAGED, anim));
			m_reversedAnimMap.insert(pair<ActorState, Animation*>(BATTLE_DAMAGED, rAnim));
			break;

		case BATTLE_DEAD:
			InitDelaysOfAnim("Battle/battle_dead", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_dead"), delays, false, false);
			rAnim = new Animation(string("Actors/") + m_name + string("/Battle/battle_dead"), delays, true, false);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_DEAD, anim));
			m_reversedAnimMap.insert(pair<ActorState, Animation*>(BATTLE_DEAD, rAnim));
			break;

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

	if(m_direction == RIGHT)
	{
		m_animMap[m_state]->Update();
		m_animMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
		m_animMap[m_state]->GetCurrentSprite().Display();
		//#ifdef _DEBUG
			m_animMap[m_state]->GetCurrentSprite().DisplayExtension();
		//#endif
	}
	else
	{
		m_reversedAnimMap[m_state]->Update();
		m_reversedAnimMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
		m_reversedAnimMap[m_state]->GetCurrentSprite().Display();
		//#ifdef _DEBUG
			m_reversedAnimMap[m_state]->GetCurrentSprite().DisplayExtension();
		//#endif
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

	m_animMap[m_state]->Stop();
	m_reversedAnimMap[m_state]->Stop();

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

Animation* MMBNBattleActor::GetCurrentAnim()
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
