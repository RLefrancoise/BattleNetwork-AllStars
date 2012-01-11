#include "MMBNTitle.h"
#include "ImgManager.h"
#include "SndManager.h"
#include "FontManager.h"
#include "Random.h"
#include "Logger.h"
#include "GameSystem.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
MMBNTitle::MMBNTitle()
{
	LOG("Create MMBNTitle Screen")
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

MMBNTitle::~MMBNTitle()
{
	LOG("Destroy MMBNTitle Screen")
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INITIALIZE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNTitle::Initialize()
{
#ifdef _DEBUG
	LOG("Initialize MMBNTitle")
#endif
	
	//bg
	m_bg = Animation::Load("Backgrounds/Title");
	m_bg->SetPosition(0,0);
	
	//picture
	m_picture = ImgManager::GetImage("Title/mmbn_title.png");
	m_picture->x = 240 - m_picture->sizeX / 2;
	m_picture->y = 5;
	
	//menu
	unsigned int menu_y = 204;
	
	m_newGame.SetFont(GameSystem::GetBattleFont());
	m_newGame = "NEW GAME";
	m_newGame.SetPosition(240 - m_newGame.GetStringWidth() / 2, menu_y - m_newGame.GetStringHeight());
	
	m_loadGame.SetFont(GameSystem::GetBattleFont());
	m_loadGame = "CONTINUE";
	m_loadGame.SetPosition(240 - m_loadGame.GetStringWidth() / 2, menu_y + 5 + m_newGame.GetStringHeight() - m_loadGame.GetStringHeight());
	
	m_exitGame.SetFont(GameSystem::GetBattleFont());
	m_exitGame = "EXIT";
	m_exitGame.SetPosition(240 - m_exitGame.GetStringWidth() / 2, menu_y + 10 + m_newGame.GetStringHeight() + m_loadGame.GetStringHeight() - m_exitGame.GetStringHeight());
	
	//cursor
	unsigned int max_width = m_newGame.GetStringWidth();
	if(max_width < m_loadGame.GetStringWidth()) max_width = m_loadGame.GetStringWidth();
	if(max_width < m_exitGame.GetStringWidth()) max_width = m_exitGame.GetStringWidth();
	
	m_cursor = ImgManager::GetImage("System/Menus/cursor.png");
	m_cursor->x = 240 - (max_width / 2) - m_cursor->sizeX - 2;
	m_cursor->y = m_newGame.GetPosition().y + abs((int) (m_cursor->sizeY - m_newGame.GetStringHeight())) / 2;

	m_menu = NEW_GAME;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNTitle::Destroy()
{
	SndManager::StopBGM(0);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void MMBNTitle::Display()
{
	
	//+++++++++++++++++++++++++++++++++++++
	// GRAPHICS
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BACKGROUND
	//======================================
	m_bg->Update();
	m_bg->Display();
	
	//======================================
	// PICTURE
	//======================================
	oslDrawImage(m_picture);
	
	//======================================
	// MENUS
	//======================================
	m_newGame.Display();
	m_loadGame.Display();
	m_exitGame.Display();
	
	//======================================
	// CURSOR
	//======================================
	oslDrawImage(m_cursor);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int MMBNTitle::Update()
{

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
			m_cursor->y = m_loadGame.GetPosition().y + abs((int) (m_cursor->sizeY - m_loadGame.GetStringHeight())) / 2;
		}
		else if(m_menu == LOAD_GAME)
		{
			m_menu = EXIT_GAME;
			m_cursor->y = m_exitGame.GetPosition().y + abs((int) (m_cursor->sizeY - m_exitGame.GetStringHeight())) / 2;
		}
	}

	//HAUT
	if(k->pressed.up)
	{
		if(m_menu == LOAD_GAME)
		{
			m_menu = NEW_GAME;
			m_cursor->y = m_newGame.GetPosition().y + abs((int) (m_cursor->sizeY - m_newGame.GetStringHeight())) / 2;
		}
		else if(m_menu == EXIT_GAME)
		{
			m_menu = LOAD_GAME;
			m_cursor->y = m_loadGame.GetPosition().y + abs((int) (m_cursor->sizeY - m_loadGame.GetStringHeight())) / 2;
		}
	}
	
	// X
	if(k->pressed.cross)
	{
		if(m_menu == NEW_GAME)
			return SCREEN_FIELDMAP;
		else if(m_menu == LOAD_GAME)
			return SCREEN_CHARACTER_SELECT;
		else
			return SCREEN_EXIT;
		
	}

	//+++++++++++++++++++++++++++++++++++++
	// SOUND
	//+++++++++++++++++++++++++++++++++++++
	//======================================
	// BGM
	//======================================
	SndManager::PlayBGM("Title_Screen.bgm", 0, true);

	//par défaut, on reste sur le même écran
	return SCREEN_TITLE;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// RUN
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int MMBNTitle::Run()
{
	int screen = Update();
	if(screen == SCREEN_TITLE) Display();

	return screen;
}


