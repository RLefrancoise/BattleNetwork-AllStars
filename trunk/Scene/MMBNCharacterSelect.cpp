#include "MMBNCharacterSelect.h"

MMBNCharacterSelect::MMBNCharacterSelect()
{
	#ifdef _DEBUG
		LOG("Create Character Select Screen")
	#endif
}

MMBNCharacterSelect::~MMBNCharacterSelect()
{
	#ifdef _DEBUG
		LOG("Destroy Character Select Screen")
	#endif
}

int	MMBNCharacterSelect::Run()
{
	int screen = Update();
	if(screen == SCREEN_CHARACTER_SELECT) Display();

	return screen;
}

void MMBNCharacterSelect::Initialize()
{
	m_bg				= Animation::Load("Backgrounds/CharacterSelect")		;
	m_plug_in			= Animation::Load("Backgrounds/PlugIn", false, false)	;
		
	//m_actor_cursor															;
	//m_enemy_cursor															;
		
	m_current_bgm		= 0														;
	
	m_can_plug_in		= false													;
}

void MMBNCharacterSelect::Destroy()
{
	SndManager::StopBGM(0);
}


int MMBNCharacterSelect::Update()
{
	OSL_CONTROLLER* k = oslReadKeys();
	
	if(m_can_plug_in)
	{
		if(m_plug_in->IsOver()) return SCREEN_BATTLEMAP;
	}
	else
	{
		if(k->pressed.triangle) return SCREEN_TITLE;
		if(k->pressed.cross) m_can_plug_in = true;
	}
	
	return SCREEN_CHARACTER_SELECT;
	
}

void MMBNCharacterSelect::Display()
{
	if(m_can_plug_in)
	{
		m_plug_in->Update();
		m_plug_in->Display();
		
		return;
	}
	
	
	m_bg->Update();
	m_bg->Display();
	
	SndManager::PlayBGM("Digital_Strider.bgm", 0, true);

}