#include "MMBNCharacterSelect.h"
#include "GameSystem.h"

using namespace std;

CharacterSelectInfo::CharacterSelectInfo()
{
	m_name = "";
	m_icon = NULL;
	m_body = NULL;
}

CharacterSelectInfo::CharacterSelectInfo(string name, OSL_IMAGE* icon, OSL_IMAGE* body)
{
	m_name = name;
	m_icon = icon;
	m_body = body;
}







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
	//backgrounds
	m_bg				= Animation::Load("Backgrounds/CharacterSelect")		;
	m_plug_in			= Animation::Load("Backgrounds/PlugIn", false, false)	;
		
	//cursors
	m_actor_cursor		= Animation::Load("System/Menus/CharacterSelect/Cursor");
	m_enemy_cursor		= Animation::Load("System/Menus/CharacterSelect/Cursor");
		
	//pictures
	m_chara_frame		= ImgManager::GetImage("System/Menus/CharacterSelect/chara_frame.png");
	
	
	//characters
	ifstream in_actors( "Actors/actors.txt" , ifstream::in);
		
	if(!in_actors.good())
	{
		LOG("Impossible de trouver le fichier Actors/actors.txt");
		oslQuit();
	}
	
	string line;
	while(getline(in_actors, line))
	{
		vector<string> v = StringUtils::Split(line, ":");
		
		#ifdef _DEBUG
		LOG("Load character: " + v[0])
		#endif

		CharacterSelectInfoPtr csi(new CharacterSelectInfo(v[0], ImgManager::GetImage("Actors/" + v[0] + "/icon.png"), NULL));
		
		m_characters.push_back(csi);
		
		/*m_characters.back()->m_name = v[0];
		m_characters.back()->m_icon = ImgManager::GetImage("Actors/" + v[0] + "/icon.png");
		m_characters.back()->m_body = NULL;*/
		#ifdef _DEBUG
		LOG("Character loaded")
		#endif
	}
	
	in_actors.close();
		
	//characters data
	/*for(unsigned int i(0) ; i < GameSystem::GetActorsNames().size() ; i++)
	{
		#ifdef _DEBUG
		LOG("Found character: " + GameSystem::GetActorsNames()[i])
		#endif

		m_characters.push_back(csi);
		
		m_characters.back().name = GameSystem::GetActorsNames()[i];
		m_characters.back().icon = ImgManager::GetImage("Actors/" + GameSystem::GetActorsNames()[i] + "/icon.png");
		m_characters.back().body = NULL;
	}*/
	
	/*int begin_x = 240 - (CHARA_PER_LINE * CHARA_ICON_SIZE) / 2;
	int begin_y = 136 - ( (m_characters.size() % CHARA_PER_LINE) * CHARA_ICON_SIZE ) / 2;
	
	for(unsigned int i(0) ; i < m_characters.size() ; ++i)
	{
		Vector2i v;
		v.x = begin_x + CHARA_ICON_SIZE * (i % CHARA_PER_LINE);
		v.y = begin_y + CHARA_ICON_SIZE * (i / CHARA_PER_LINE);
		m_chara_positions.push_back(v);
	}*/
	
	//variables
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
	
	//background
	m_bg->Update();
	m_bg->Display();
	
	//characters
	/*for(unsigned int i(0) ; i < m_characters.size() ; ++i)
	{
		oslDrawImageXY(m_characters[i]->m_icon, m_chara_positions[i].x, m_chara_positions[i].y);
	}*/
	
	SndManager::PlayBGM("Digital_Strider.bgm", 0, true);

}