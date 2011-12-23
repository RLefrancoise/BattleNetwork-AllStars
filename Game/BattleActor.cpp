#include "BattleActor.h"
#include "ImgManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

//float rapport = sqrt(2.0f) / 2;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
BattleActor::BattleActor()
{
	m_posX = 0;
	m_posY = 0;
	m_offX = 0;
	m_offY = 0;
}

BattleActor::BattleActor(string name)
{
	m_name = name; // /!\ à mettre au début car les autres fonctions en ont besoin

	//===============================
	// ANIMATIONS
	//===============================
	// BATTLE
	//===============================
	// STANDING
	//-------------------------------
	LoadStateAnim(BATTLE_STANDING_LEFT);
	LoadStateAnim(BATTLE_STANDING_RIGHT);

	//-------------------------------
	// WALKING
	//-------------------------------
	LoadStateAnim(BATTLE_WALKING_LEFT);
	LoadStateAnim(BATTLE_WALKING_RIGHT);

	//-------------------------------

	m_direction = RIGHT;
	m_state = BATTLE_STANDING_RIGHT;

	m_lv	= 1							;
	m_hp	= 100						;
	m_maxHp	= 100						;
	m_str	= 10						;
	m_def	= 10						;
	m_int	= 10						;
	m_xVel	= 2 * 64					;
	m_yVel	= 2 * 40					;


	m_lifeBar = new ActorLifeBar(this); // /!\ mettre m_name = name avant !

	m_posX = 0;
	m_posY = 0;

	m_actorSurface = 20;

	m_colliRect.AddPoint(Vector2f(-m_actorSurface / 2, -m_actorSurface / 2));
	m_colliRect.AddPoint(Vector2f(m_actorSurface / 2, -m_actorSurface / 2));
	m_colliRect.AddPoint(Vector2f(m_actorSurface / 2, m_actorSurface / 2));
	m_colliRect.AddPoint(Vector2f(-m_actorSurface / 2, m_actorSurface / 2));

	float rac200 = sqrt(200.0f);

	m_colliDiamond.AddPoint(Vector2f(0, -rac200 / 2));
	m_colliDiamond.AddPoint(Vector2f(rac200, 0));
	m_colliDiamond.AddPoint(Vector2f(0, rac200 / 2));
	m_colliDiamond.AddPoint(Vector2f(-rac200, 0));
	

}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

BattleActor::~BattleActor()
{
	if(m_lifeBar) delete m_lifeBar;

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
BattleActor* BattleActor::Load(std::string name)
{
	return new BattleActor(name);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INIT DELAYS OF ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void BattleActor::InitDelaysOfAnim(string name, vector<int> &delays)
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
void BattleActor::LoadStateAnim(ActorState state)
{
	Animation* anim = NULL;
	std::vector<int> delays;

	switch(state)
	{
		//===============================
		// BATTLE
		//===============================
		case BATTLE_STANDING_LEFT:
			InitDelaysOfAnim("Battle/battle_standing", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_standing"), delays, true);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_STANDING_LEFT, anim));
			break;

		case BATTLE_STANDING_RIGHT:
			InitDelaysOfAnim("Battle/battle_standing", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_standing"), delays, false);
			
			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_STANDING_RIGHT, anim));
			break;

		case BATTLE_WALKING_LEFT:
			InitDelaysOfAnim("Battle/battle_run", delays);

			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_run"), delays, true);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_WALKING_LEFT, anim));
			break;

		case BATTLE_WALKING_RIGHT:
			InitDelaysOfAnim("Battle/battle_run", delays);

			anim = new Animation(string("Actors/") + m_name + string("/Battle/battle_run"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(BATTLE_WALKING_RIGHT, anim));
			break;

	}
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void BattleActor::Display(float offX, float offY)
{

	m_offX = offX;
	m_offY = offY;

		m_colliDiamond.SetPosition(m_posX + offX, m_posY + offY);
		m_colliDiamond.Display();
		
		m_animMap[m_state]->Update();
		m_animMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
		m_animMap[m_state]->GetCurrentSprite().Display();
		//#ifdef _DEBUG
			m_animMap[m_state]->GetCurrentSprite().DisplayExtension();
		//#endif

}

void BattleActor::Move(float x, float y)
{
	this->SetPosition(m_posX + x, m_posY + y);
}

void BattleActor::SetDirection(ActorDirection direction)
{
	if(m_direction == direction) return;

	if(direction == LEFT) m_direction = LEFT;
	else if(direction == RIGHT) m_direction = RIGHT;
	else if(direction == UP) m_direction = UP;
	else if(direction == UP_LEFT) m_direction = UP_LEFT;
	else if(direction == UP_RIGHT) m_direction = UP_RIGHT;
	else if(direction == DOWN) m_direction = DOWN;
	else if(direction == DOWN_LEFT) m_direction = DOWN_LEFT;
	else if(direction == DOWN_RIGHT) m_direction = DOWN_RIGHT;
}

void BattleActor::SetState(ActorState state)
{
	if(m_state == state) return;

	//===============================
	// BATTLE
	//===============================
	if(state == BATTLE_STANDING_LEFT) m_state = BATTLE_STANDING_LEFT;
	else if(state == BATTLE_STANDING_RIGHT) m_state = BATTLE_STANDING_RIGHT;
	else if(state == BATTLE_WALKING_LEFT) m_state = BATTLE_WALKING_LEFT;
	else if(state == BATTLE_WALKING_RIGHT) m_state = BATTLE_WALKING_RIGHT;

	//===================================
	// ON STOPPE TOUTES LES ANIMATIONS SAUF CELLE DE L'ETAT COURANT
	//===================================
	std::map<ActorState,Animation*>::iterator it;

	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).first != state ) m_animMap[(*it).first]->Stop();

	for(it = m_reversedAnimMap.begin() ; it != m_reversedAnimMap.end() ; ++it)
		if( (*it).first != state ) m_reversedAnimMap[(*it).first]->Stop();
}

void BattleActor::SetPosition(float x, float y)
{
	m_posX = x;
	m_posY = y;
	m_colliDiamond.SetPosition(m_posX,m_posY);
}

Shape& BattleActor::GetColliShape()
{
	return m_colliDiamond;
	
}

BattleActor::ActorDirection BattleActor::GetDirection() const
{
	return m_direction;
}

BattleActor::ActorState BattleActor::GetState() const
{
	return m_state;
}

ActorLifeBar* BattleActor::GetLifeBar() const
{
	return m_lifeBar;
}

string BattleActor::GetName() const
{
	return m_name;
}

Animation* BattleActor::GetCurrentAnim()
{
	return m_animMap[m_state];
}

Vector2f BattleActor::GetPosition()
{
	return Vector2f(m_posX, m_posY);
	
}

Vector2f BattleActor::GetOffset()
{
	return Vector2f(m_offX, m_offY);
}

Vector2f BattleActor::GetPositionWithOffset()
{
	return Vector2f(m_posX + m_offX, m_posY + m_offY);
}

int BattleActor::GetLV() const
{
	return m_lv;
}

int BattleActor::GetHP() const
{
	return m_hp;
}

int BattleActor::GetMaxHP() const
{
	return m_maxHp;
}
int BattleActor::GetStr() const
{
	return m_str;
}

int BattleActor::GetDef() const
{
	return m_def;
}

int BattleActor::GetInt() const
{
	return m_int;
}

int BattleActor::GetXVel() const
{
	return m_xVel;
}

int BattleActor::GetYVel() const
{
	return m_yVel;
}

int BattleActor::GetActorSurface() const
{
	return m_actorSurface;
}


int BattleActor::IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P)
{
	Vector2f D,E;
	D.x = B.x - A.x;
	D.y = B.y - A.y;
	E.x = P.x - I.x;
	E.y = P.y - I.y;
	float t = 0, u = 0;
	double denom = D.x*E.y - D.y*E.x;
	if (denom==0)
		return -1;   // erreur, cas limite
	t = - (A.x*E.y-I.x*E.y-E.x*A.y+E.x*I.y) / denom;
	if (t<0 || t>=1)
		return 0;
	u = - (-D.x*A.y+D.x*I.y+D.y*A.x-D.y*I.x) / denom;
	if (u<0 || u>=1)
		return 0;
	return 1;
}

bool BattleActor::Collide(Shape& s)
{
	int i, j;
	Vector2f A,B,I,P;

	//on construit les segments de l'objet
	for(i=0 ; i < s.GetPointsCount() ; i++)
	{
		A = s.GetPointPosition(i);
		A.x += s.GetPosition().x;
		A.y += s.GetPosition().y;

		if (i == s.GetPointsCount() - 1)  // si c'est le dernier point, on relie au premier
		{
			B = s.GetPointPosition(0);
			B.x += s.GetPosition().x;
			B.y += s.GetPosition().y;
		}
		else           // sinon on relie au suivant.
		{
			B = s.GetPointPosition(i+1);
			B.x += s.GetPosition().x;
			B.y += s.GetPosition().y;
		}

		//on construit les segments du perso
		for(j = 0 ; j < m_colliDiamond.GetPointsCount() ; j++)
		{
			I = m_colliDiamond.GetPointPosition(j);
			I.x += (m_colliDiamond.GetPosition().x);
			I.y += (m_colliDiamond.GetPosition().y);

			if (j == m_colliDiamond.GetPointsCount() - 1)  // si c'est le dernier point, on relie au premier
			{
				P = m_colliDiamond.GetPointPosition(0);
				P.x += (m_colliDiamond.GetPosition().x);
				P.y += (m_colliDiamond.GetPosition().y);
			}
			else           // sinon on relie au suivant.
			{
				P = m_colliDiamond.GetPointPosition(j+1);
				P.x += (m_colliDiamond.GetPosition().x);
				P.y += (m_colliDiamond.GetPosition().y);
			}


			int iseg = IntersectSegments(A,B,I,P);
			if(iseg == 1) return true;
		}
	}

	return false;
}