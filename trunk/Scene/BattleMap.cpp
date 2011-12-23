#include "BattleMap.h"
#include "ImgManager.h"
#include "SndManager.h"
#include "FontManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>

#define PI 3.14159265

using namespace std;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
BattleMap::BattleMap()
{

	m_bg = ImgManager::GetImage("Backgrounds/co.png");

	//Création de la vue
	m_view.SetSize(480,272);
	m_view.SetCenter(240,161);

	m_actor = BattleActor::Load("Riku");
	m_actor->SetPosition(150, 200);

	m_actorLifeBar_x = 410;
	m_actorLifeBar_y = 270;

	m_enemy = BattleActor::Load("Roxas");
	m_enemy->SetPosition(300, 200);
	m_enemy->SetState(BattleActor::BATTLE_STANDING_LEFT);
	m_enemy->GetLifeBar()->SetExtended(false);

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

BattleMap::~BattleMap()
{
	if(m_actor) delete m_actor;
	if(m_enemy) delete m_enemy;
}




//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void BattleMap::Display()
{

	//+++++++++++++++++++++++++++++++++++++
	// GRAPHICS
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BACKGROUND
	//======================================
	oslDrawImageXY(m_bg, 0, 0);

	//======================================
	// ACTOR & ENEMIES
	//======================================
	if(m_enemy->GetPosition().y < m_actor->GetPosition().y)
	{
		m_enemy->Display();
		m_actor->Display();
	}
	else
	{
		m_actor->Display();
		m_enemy->Display();
	}
	
	//======================================
	// LIFEBARS
	//======================================
	m_actor->GetLifeBar()->Display(m_actorLifeBar_x, m_actorLifeBar_y);
	m_enemy->GetLifeBar()->Display(10, 10);

	//+++++++++++++++++++++++++++++++++++++
	// SOUND
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BGM
	//======================================
	SndManager::PlayBGM("Roxas_Theme.bgm", 0, true);

#ifdef _DEBUG
/*
	//=======================================
	// COLLISIONS
	//=======================================
	std::vector<AABBCollisionBox>::iterator it;
	for(it = m_collisions.begin() ; it != m_collisions.end() ; ++it)
	{
		sf::Shape box = sf::Shape::Rectangle( 0, 0, it->Width, it->Height, sf::Color(0,255,0), 1, sf::Color(0,255,0) );
		box.EnableFill(false);
		box.EnableOutline(true);
		box.SetPosition(it->Left, it->Top);
		App.Draw(box);
	}

	for(it = m_objects.begin() ; it != m_objects.end() ; ++it)
	{
		sf::Shape box = sf::Shape::Rectangle( 0, 0, it->Width, it->Height, sf::Color(255,0,255), 1, sf::Color(255,0,255) );
		box.EnableFill(false);
		box.EnableOutline(true);
		box.SetPosition(it->Left, it->Top);
		App.Draw(box);
	}

	//=======================================
	// ISO
	//=======================================
	sf::Vector2f actorPos = GetActorTile(m_actor->GetPosition().x, m_actor->GetPosition().y);
	ostringstream ap_oss(ostringstream::out);
	ap_oss << "Actor Tile X : " << actorPos.x << " Y : " << actorPos.y;

	m_actorTileText.SetString( string(ap_oss.str() ) );

	m_actorTileText.SetPosition(20, 100);
	App.Draw(m_actorTileText);

	*/
#endif
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int BattleMap::Update()
{
	//si on ne doit pas gérer les évènements, on vide la pile et on sort
	/*if(!m_eventsEnabled)
	{
		sf::Event Event;
		while (App.GetEvent(Event));
		return SCREEN_BATTLEMAP;
	}*/

	//=============================
	// TRAITEMENT DES EVENEMENTS
	//=============================
	OSL_CONTROLLER* k = oslReadKeys();

	if(k->pressed.triangle) return SCREEN_TITLE;

	//gestion du héros
	ActorHandle();
	
	//par défaut, on reste sur le même écran
	return SCREEN_BATTLEMAP;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int BattleMap::Run()
{
	//App.SetView(m_view);

	int screen = Update();
	if(screen == SCREEN_BATTLEMAP) Display();

	return screen;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ACTOR HANDLE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void BattleMap::ActorHandle()
{
	bool leftPressed = false;
	bool rightPressed = false;
	bool upPressed = false;
	bool downPressed = false;

	OSL_CONTROLLER* k = oslReadKeys();

	if(k->held.left) leftPressed = true;
	if(k->held.right) rightPressed = true;
	if(k->held.up) upPressed = true;
	if(k->held.down) downPressed = true;

	if(downPressed) {
		if(leftPressed)
			MoveActor(m_actor, AM_DOWN_LEFT);
		else if(rightPressed)
			MoveActor(m_actor, AM_DOWN_RIGHT);
		else
			MoveActor(m_actor, AM_DOWN);
	}
	else if(upPressed) {
		if(leftPressed)
			MoveActor(m_actor, AM_UP_LEFT);
		else if(rightPressed)
			MoveActor(m_actor, AM_UP_RIGHT);
		else
			MoveActor(m_actor, AM_UP);
	}

	else if(leftPressed) {
		MoveActor(m_actor, AM_LEFT);
	}
	else if(rightPressed) {
		MoveActor(m_actor, AM_RIGHT);
	}

	else if(!upPressed && !downPressed) {
		if(m_actor->GetDirection() == BattleActor::RIGHT)
			m_actor->SetState(BattleActor::BATTLE_STANDING_RIGHT);
		else if(m_actor->GetDirection() == BattleActor::LEFT)
			m_actor->SetState(BattleActor::BATTLE_STANDING_LEFT);
	}

	if(m_actor->GetPosition().x < 0) m_actor->SetPosition(0, m_actor->GetPosition().y);
	if(m_actor->GetPosition().y < 0) m_actor->SetPosition(m_actor->GetPosition().x, 0);
	if(m_actor->GetPosition().x > 480) m_actor->SetPosition(480, m_actor->GetPosition().y);
	if(m_actor->GetPosition().y > 272) m_actor->SetPosition(m_actor->GetPosition().x, 272);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// MOVE ACTOR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void BattleMap::MoveActor(BattleActor* actor, ActorMoving move)
{
	int depX = 0;
	int depY = 0;
	Vector2f v;

	switch(move)
	{
		case AM_DOWN_LEFT :
			actor->SetDirection(BattleActor::LEFT);
			actor->SetState(BattleActor::BATTLE_WALKING_LEFT);

			depX = -actor->GetXVel() / Variables::GetFPS();
			depY = actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y + depY);
			break;

		case AM_DOWN_RIGHT :
			actor->SetDirection(BattleActor::RIGHT);
			actor->SetState(BattleActor::BATTLE_WALKING_RIGHT);

			depX = actor->GetXVel() / Variables::GetFPS();
			depY = actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y + depY);
			break;

		case AM_UP_LEFT :
			actor->SetDirection(BattleActor::LEFT);
			actor->SetState(BattleActor::BATTLE_WALKING_LEFT);

			depX = -actor->GetXVel() / Variables::GetFPS();
			depY = -actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y + depY);
			break;

		case AM_UP_RIGHT :
			actor->SetDirection(BattleActor::RIGHT);
			actor->SetState(BattleActor::BATTLE_WALKING_RIGHT);

			depX = actor->GetXVel() / Variables::GetFPS();
			depY = -actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y + depY);
			break;

		case AM_LEFT :
			actor->SetDirection(BattleActor::LEFT);
			actor->SetState(BattleActor::BATTLE_WALKING_LEFT);

			depX = -actor->GetXVel() / Variables::GetFPS();
			depY = 0;
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y);
			break;

		case AM_RIGHT :
			actor->SetDirection(BattleActor::RIGHT);
			actor->SetState(BattleActor::BATTLE_WALKING_RIGHT);

			depX = actor->GetXVel() / Variables::GetFPS();
			depY = 0;
			actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y);
			break;

		case AM_UP :
			if(actor->GetDirection() == BattleActor::LEFT)
				actor->SetState(BattleActor::BATTLE_WALKING_LEFT);
			else if(actor->GetDirection() == BattleActor::RIGHT)
				actor->SetState(BattleActor::BATTLE_WALKING_RIGHT);
			
			depX = 0;
			depY = -actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x, actor->GetPosition().y + depY);
			break;

		case AM_DOWN :
			if(actor->GetDirection() == BattleActor::LEFT)
				actor->SetState(BattleActor::BATTLE_WALKING_LEFT);
			else if(actor->GetDirection() == BattleActor::RIGHT)
				actor->SetState(BattleActor::BATTLE_WALKING_RIGHT);

			depX = 0;
			depY = actor->GetYVel() / Variables::GetFPS();
			actor->SetPosition(actor->GetPosition().x, actor->GetPosition().y + depY);
			break;
	}

	if(actor->Collide(m_enemy->GetColliShape()))
		actor->Move(-depX, -depY);

	/*v = GetActorTile(actor->GetPosition().x, actor->GetPosition().y);

	if(v.x < 0) actor->SetPosition(actor->GetPosition().x + depX, actor->GetPosition().y);
	if(v.x > m_xCellNb) actor->SetPosition(actor->GetPosition().x - depX, actor->GetPosition().y);
	if(v.y < 0) actor->SetPosition(actor->GetPosition().x, actor->GetPosition().y - depY);
	if(v.y > m_yCellNb) actor->SetPosition(actor->GetPosition().x, actor->GetPosition().y + depY);*/
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ENABLE EVENTS
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/*void BattleMap::EnableEvents(bool b)
{
	if(b) m_eventsEnabled = true;
	else m_eventsEnabled = false;
}*/
