#include "Title.h"
#include "ImgManager.h"
#include "SndManager.h"
#include "FontManager.h"
#include "Random.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
Title::Title()
{
	LOG("Create Title Screen")
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

Title::~Title()
{
	LOG("Destroy Title Screen")
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Title::Initialize()
{
#ifdef _DEBUG
	LOG("Initialize Title")
#endif
	
	m_currentAlpha = 0;

	m_bg = ImgManager::GetImage("Title/title.png");
	m_bg->x = 0;
	m_bg->y = 0;

	int random = Random::RandomInt(1,3);
	if(random == 1)
		m_char = ImgManager::GetImage("Title/character.png");
	else
		m_char = ImgManager::GetImage("Title/character2.png");

	
	m_char->x = 480 - m_char->stretchX;
	m_char->y = 0;
	

	m_newGame = ImgManager::GetImage("Title/new_game.png");
	m_newGame->x = 25;
	m_newGame->y = 272 - 2 * m_newGame->stretchY - 10;
	

	m_loadGame = ImgManager::GetImage("Title/load_game.png");
	m_loadGame->x = 25;
	m_loadGame->y = 272 - m_loadGame->stretchY - 10;
	

	m_cursor = ImgManager::GetImage("Title/cursor.png");
	m_cursor->x = 27;
	m_cursor->y = 272 - 2 * m_newGame->stretchY - 6;

	m_menu = NEW_GAME;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Title::Destroy()
{
	SndManager::StopBGM(0);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Title::Display()
{
	
	//+++++++++++++++++++++++++++++++++++++
	// GRAPHICS
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BACKGROUND
	//======================================
	oslDrawImage(m_bg);
	//======================================
	// CHARACTER
	//======================================
	oslSetAlpha(OSL_FX_ALPHA, m_currentAlpha);
	oslDrawImage(m_char);
	oslSetAlpha(OSL_FX_ALPHA, m_currentAlpha);
	//======================================
	// MENUS
	//======================================
	oslSetAlpha(OSL_FX_ALPHA, 220);
	oslDrawImage(m_newGame);
	oslDrawImage(m_loadGame);
	oslSetAlpha(OSL_FX_ALPHA, 220);
	oslDrawImage(m_cursor);

}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int Title::Update()
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

	//BAS
	if(k->pressed.down)
	{
		if(m_menu == NEW_GAME)
		{
			m_menu = LOAD_GAME;
			m_cursor->y += m_newGame->stretchY;
			//m_cursor.SetPosition(m_cursor.GetPosition().x, m_cursor.GetPosition().y + m_newGame.GetSubRect().Height);
		}
	}

	//HAUT
	if(k->pressed.up)
	{
		if(m_menu == LOAD_GAME)
		{
			m_menu = NEW_GAME;
			m_cursor->y -= m_newGame->stretchY;
			//m_cursor.SetPosition(m_cursor.GetPosition().x, m_cursor.GetPosition().y - m_newGame.GetSubRect().Height);
		}
	}
	
	// X
	if(k->pressed.cross)
	{
		if(m_menu == NEW_GAME)
			return SCREEN_FIELDMAP;
		
		else if(m_menu == LOAD_GAME)
			return SCREEN_BATTLEMAP;
		
	}

	// triangle
	if(k->pressed.triangle)
		return SCREEN_EXIT;
		
	//=============================
	// MISE A JOUR DE L'AFFICHAGE
	//=============================
	// Affichage du personnage avec
	// gestion de la transparence
	//-----------------------------
	if(!m_clock.is_started()) m_clock.start();

	if(m_currentAlpha < 255)
	{
		float elapsedTime = m_clock.get_ticks();
		if(elapsedTime > 125)
		{
			m_currentAlpha += (255 / 20);
			if(m_currentAlpha > 255) m_currentAlpha = 255;
			m_clock.stop();
			m_clock.start();
			//m_char.SetColor(sf::Color(255,255,255,m_currentAlpha));
		}
	}
	
	//+++++++++++++++++++++++++++++++++++++
	// SOUND
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BGM
	//======================================
	SndManager::PlayBGM("Dearly_Beloved.bgm", 0, true);

	//par défaut, on reste sur le même écran
	return SCREEN_TITLE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int Title::Run()
{
	int screen = Update();
	if(screen == SCREEN_TITLE) Display();

	return screen;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ENABLE EVENTS
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/*void Title::EnableEvents(bool b)
{
	if(b) m_eventsEnabled = true;
	else m_eventsEnabled = false;
}*/

