#include "Actor.h"
#include "ImgManager.h"
#include "Logger.h"
#include "FieldMap.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

float rapport = sqrt(2.0f) / 2;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
Actor::Actor() : FieldMapElement()
{
	m_posX = 0;
	m_posY = 0;
	m_offX = 0;
	m_offY = 0;
}

Actor::Actor(string name) : FieldMapElement()
{
	m_name = name; // /!\ à mettre au début car les autres fonctions en ont besoin

	for(unsigned int i = 0 ; i < PROPERTIES_NB ; i++)
		m_properties[i] = true;

	//-------------------------------

	m_direction = DOWN;
	m_state = FIELD_STANDING_DOWN;

	m_lv	= 1							;
	m_hp	= 100						;
	m_maxHp	= 100						;
	m_str	= 10						;
	m_def	= 10						;
	m_int	= 10						;
	m_xVel	= 2 * 64					;
	m_yVel	= 2 * 40					;

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

	m_colliDiamond.SetColor(RGB(255,0,0));
	

}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

Actor::~Actor()
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
Actor* Actor::Load(std::string name, bool* properties)
{
	Actor* a = new Actor(name);
	if(properties != NULL)
	{ 
		a->m_properties[WALKABLE] = properties[WALKABLE];
		a->m_properties[TURNABLE] = properties[TURNABLE];
	}

	//Load Animations for each state according to the properties
	a->LoadStates();

	return a;
}

void Actor::LoadStates()
{
	if(m_properties[TURNABLE])
	{
		if(m_properties[WALKABLE])
		{
			LoadStateAnim(FIELD_RUN_UP);
			LoadStateAnim(FIELD_RUN_LEFT);
			LoadStateAnim(FIELD_RUN_RIGHT);
			LoadStateAnim(FIELD_RUN_DOWN);
			LoadStateAnim(FIELD_RUN_DOWN_LEFT);
			LoadStateAnim(FIELD_RUN_DOWN_RIGHT);
			LoadStateAnim(FIELD_RUN_UP_LEFT);
			LoadStateAnim(FIELD_RUN_UP_RIGHT);
		}

		LoadStateAnim(FIELD_STANDING_UP);
		LoadStateAnim(FIELD_STANDING_LEFT);
		LoadStateAnim(FIELD_STANDING_RIGHT);
		LoadStateAnim(FIELD_STANDING_DOWN_LEFT);
		LoadStateAnim(FIELD_STANDING_DOWN_RIGHT);
		LoadStateAnim(FIELD_STANDING_UP_LEFT);
		LoadStateAnim(FIELD_STANDING_UP_RIGHT);
	}		
	
	//the default state, always initialized
	LoadStateAnim(FIELD_STANDING_DOWN);
	
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INIT DELAYS OF ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Actor::InitDelaysOfAnim(string name, vector<int> &delays)
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
void Actor::LoadStateAnim(ActorState state)
{
	Animation* anim = NULL;
	std::vector<int> delays;

	switch(state)
	{
		//===============================
		// FIELD
		//===============================
		case FIELD_STANDING_UP:
			InitDelaysOfAnim("Field/field_standing_up", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_up"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_UP, anim));
			break;

		case FIELD_RUN_UP:
			InitDelaysOfAnim("Field/field_run_up", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_up"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_UP, anim));
			break;

		case FIELD_STANDING_DOWN:
			InitDelaysOfAnim("Field/field_standing_down", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_down"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_DOWN, anim));
			break;

		case FIELD_RUN_DOWN:
			InitDelaysOfAnim("Field/field_run_down", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_down"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_DOWN, anim));
			break;

		case FIELD_STANDING_LEFT:
			InitDelaysOfAnim("Field/field_standing_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_LEFT, anim));
			break;

		case FIELD_RUN_LEFT:
			InitDelaysOfAnim("Field/field_run_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_LEFT, anim));
			break;

		case FIELD_STANDING_RIGHT:
			InitDelaysOfAnim("Field/field_standing_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_RIGHT, anim));
			break;

		case FIELD_RUN_RIGHT:
			InitDelaysOfAnim("Field/field_run_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_RIGHT, anim));
			break;

		case FIELD_STANDING_DOWN_LEFT:
			InitDelaysOfAnim("Field/field_standing_down_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_down_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_DOWN_LEFT, anim));
			break;

		case FIELD_RUN_DOWN_LEFT:
			InitDelaysOfAnim("Field/field_run_down_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_down_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_DOWN_LEFT, anim));
			break;

		case FIELD_STANDING_DOWN_RIGHT:
			InitDelaysOfAnim("Field/field_standing_down_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_down_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_DOWN_RIGHT, anim));
			break;

		case FIELD_RUN_DOWN_RIGHT:
			InitDelaysOfAnim("Field/field_run_down_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_down_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_DOWN_RIGHT, anim));
			break;

		case FIELD_STANDING_UP_LEFT:
			InitDelaysOfAnim("Field/field_standing_up_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_up_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_UP_LEFT, anim));
			break;

		case FIELD_RUN_UP_LEFT:
			InitDelaysOfAnim("Field/field_run_up_left", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_up_left"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_UP_LEFT, anim));
			break;

		case FIELD_STANDING_UP_RIGHT:
			InitDelaysOfAnim("Field/field_standing_up_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_standing_up_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_STANDING_UP_RIGHT, anim));
			break;

		case FIELD_RUN_UP_RIGHT:
			InitDelaysOfAnim("Field/field_run_up_right", delays);
			
			anim = new Animation(string("Actors/") + m_name + string("/Field/field_run_up_right"), delays, false);

			m_animMap.insert(pair<ActorState, Animation*>(FIELD_RUN_UP_RIGHT, anim));
			break;
	}
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Actor::Display(float offX, float offY)
{

	m_offX = offX;
	m_offY = offY;

	//m_colliDiamond.SetPosition(m_posX, m_posY);
	m_colliDiamond.Display(offX, offY);
	
	m_animMap[m_state]->Update();
	m_animMap[m_state]->GetCurrentSprite().SetPosition(m_posX + offX, m_posY + offY);
	m_animMap[m_state]->GetCurrentSprite().Display();
	//#ifdef _DEBUG
		m_animMap[m_state]->GetCurrentSprite().DisplayExtension();
	//#endif

}

void Actor::Move(float x, float y)
{
	this->SetPosition(m_posX + x, m_posY + y);
}

void Actor::SetDirection(ActorDirection direction)
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

void Actor::SetState(ActorState state)
{
	if(m_state == state) return;

	//===============================
	// FIELD
	//===============================
	else if(state == FIELD_STANDING_UP) m_state = FIELD_STANDING_UP;
	else if(state == FIELD_RUN_UP) m_state = FIELD_RUN_UP;
	else if(state == FIELD_STANDING_DOWN) m_state = FIELD_STANDING_DOWN;
	else if(state == FIELD_RUN_DOWN) m_state = FIELD_RUN_DOWN;
	else if(state == FIELD_STANDING_LEFT) m_state = FIELD_STANDING_LEFT;
	else if(state == FIELD_RUN_LEFT) m_state = FIELD_RUN_LEFT;
	else if(state == FIELD_STANDING_RIGHT) m_state = FIELD_STANDING_RIGHT;
	else if(state == FIELD_RUN_RIGHT) m_state = FIELD_RUN_RIGHT;
	else if(state == FIELD_STANDING_DOWN_LEFT) m_state = FIELD_STANDING_DOWN_LEFT;
	else if(state == FIELD_RUN_DOWN_LEFT) m_state = FIELD_RUN_DOWN_LEFT;
	else if(state == FIELD_STANDING_DOWN_RIGHT) m_state = FIELD_STANDING_DOWN_RIGHT;
	else if(state == FIELD_RUN_DOWN_RIGHT) m_state = FIELD_RUN_DOWN_RIGHT;
	else if(state == FIELD_STANDING_UP_LEFT) m_state = FIELD_STANDING_UP_LEFT;
	else if(state == FIELD_RUN_UP_LEFT) m_state = FIELD_RUN_UP_LEFT;
	else if(state == FIELD_STANDING_UP_RIGHT) m_state = FIELD_STANDING_UP_RIGHT;
	else if(state == FIELD_RUN_UP_RIGHT) m_state = FIELD_RUN_UP_RIGHT;

	//===================================
	// ON STOPPE TOUTES LES ANIMATIONS SAUF CELLE DE L'ETAT COURANT
	//===================================
	std::map<ActorState,Animation*>::iterator it;

	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).first != state ) m_animMap[(*it).first]->Stop();

	for(it = m_reversedAnimMap.begin() ; it != m_reversedAnimMap.end() ; ++it)
		if( (*it).first != state ) m_reversedAnimMap[(*it).first]->Stop();
}

void Actor::SetPosition(float x, float y)
{
	m_posX = x;
	m_posY = y;
	m_colliDiamond.SetPosition(m_posX,m_posY);
}

Shape& Actor::GetColliShape()
{
	return m_colliDiamond;
	
}

Actor::ActorDirection Actor::GetDirection() const
{
	return m_direction;
}

Actor::ActorState Actor::GetState() const
{
	return m_state;
}

string Actor::GetName() const
{
	return m_name;
}

Animation* Actor::GetCurrentAnim()
{
	return m_animMap[m_state];
}

Vector2f& Actor::GetPosition()
{
	m_pos.x = m_posX;
	m_pos.y = m_posY;
	return m_pos;
	//return Vector2f(m_posX, m_posY);
	
}

Vector2f& Actor::GetTilePosition()
{
	if(m_map == NULL)
	{
		m_tile_pos.x = 0;
		m_tile_pos.y = 0;
	}
	else
		m_tile_pos = m_map->GetActorTile(m_pos.x, m_pos.y);
	
	
	return m_tile_pos;
}

Vector2f Actor::GetOffset()
{
	return Vector2f(m_offX, m_offY);
}

Vector2f Actor::GetPositionWithOffset()
{
	return Vector2f(m_posX + m_offX, m_posY + m_offY);
}

int Actor::GetLV() const
{
	return m_lv;
}

int Actor::GetHP() const
{
	return m_hp;
}

int Actor::GetMaxHP() const
{
	return m_maxHp;
}
int Actor::GetStr() const
{
	return m_str;
}

int Actor::GetDef() const
{
	return m_def;
}

int Actor::GetInt() const
{
	return m_int;
}

int Actor::GetXVel() const
{
	return m_xVel;
}

int Actor::GetYVel() const
{
	return m_yVel;
}

int Actor::GetActorSurface() const
{
	return m_actorSurface;
}


/*int Actor::IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P)
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
}*/

bool Actor::Collide(Collidable* o)
{
	int i, j;
	Vector2f A,B,I,P;

	//on construit les segments de l'objet
	for(i=0 ; i < o->GetColliShape().GetPointsCount() ; i++)
	{
		A = o->GetColliShape().GetPointPosition(i);
		A.x += o->GetColliShape().GetPosition().x;
		A.y += o->GetColliShape().GetPosition().y;

		if (i == o->GetColliShape().GetPointsCount() - 1)  // si c'est le dernier point, on relie au premier
		{
			B = o->GetColliShape().GetPointPosition(0);
			B.x += o->GetColliShape().GetPosition().x;
			B.y += o->GetColliShape().GetPosition().y;
		}
		else           // sinon on relie au suivant.
		{
			B = o->GetColliShape().GetPointPosition(i+1);
			B.x += o->GetColliShape().GetPosition().x;
			B.y += o->GetColliShape().GetPosition().y;
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
			if( iseg == 1 ) return true;
		}
	}

	return false;
}