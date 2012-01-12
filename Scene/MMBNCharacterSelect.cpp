#include "MMBNCharacterSelect.h"
#include "GameSystem.h"

using namespace std;

CharacterSelectInfo::CharacterSelectInfo()
{
	m_name = "";
	m_icon = NULL;
	m_body = NULL;
}

CharacterSelectInfo::CharacterSelectInfo(string name, OSL_IMAGE* icon, OSL_IMAGE* body, Vector2i pos)
{
	m_name = name;
	m_icon = icon;
	m_body = body;
	m_position = pos;
}

CharacterSelectInfo::~CharacterSelectInfo()
{
}

string CharacterSelectInfo::GetName()
{
	return m_name;
}

OSL_IMAGE* CharacterSelectInfo::GetIcon()
{
	return m_icon;
}

OSL_IMAGE* CharacterSelectInfo::GetBody()
{
	return m_body;
}

Vector2i CharacterSelectInfo::GetPosition()
{
	return m_position;
}


//int	MMBNCharacterSelect::CHARA_PER_LINE = 5			;
//int	MMBNCharacterSelect::CHARA_ICON_SIZE = 32		;


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
	CHARA_PER_LINE = 5			;
	CHARA_ICON_SIZE = 38		;

	//backgrounds
	m_bg				= Animation::Load("Backgrounds/CharacterSelect")		;
	m_plug_in			= Animation::Load("Backgrounds/PlugIn", false, false)	;
		
	//cursors
	m_actor_cursor		= Animation::Load("System/Menus/CharacterSelect/Cursor");
	m_enemy_cursor		= Animation::Load("System/Menus/CharacterSelect/Cursor");
		
	//pictures
	m_chara_frame		= ImgManager::GetImage("System/Menus/CharacterSelect/chara_frame.png");
	
		
	//character data
	vector<string>& v = GameSystem::GetActorsNames();
	
	boost::array<CharactersInfo::index,1> shape = { { v.size() } };
	CharactersInfoPtr ptr(new CharactersInfo(shape));
	m_characters.swap(ptr);
	
	m_chara_nb = v.size();
	
	int begin_x = 240 - (CHARA_PER_LINE * CHARA_ICON_SIZE) / 2;
	//int begin_y = 136 - ( ( m_chara_nb % CHARA_PER_LINE) * CHARA_ICON_SIZE ) / 2;
	int begin_y = 136 - ( ( m_chara_nb % CHARA_PER_LINE) * CHARA_ICON_SIZE ) / 2;
	
	#ifdef _DEBUG
	ostringstream oss(ostringstream::out);
	oss << "begin_x: " << begin_x << " begin_y: " << begin_y;
	LOG(oss.str())
	#endif
	
	#ifdef _DEBUG
	LOG("Load characters")
	#endif
		
	//characters data
	for(unsigned int i(0) ; i < m_chara_nb ; i++)
	{
		#ifdef _DEBUG
		LOG("Found character: " + v[i])
		#endif

		Vector2i pos;
		pos.x = begin_x + CHARA_ICON_SIZE * (i % CHARA_PER_LINE);
		pos.y = begin_y + CHARA_ICON_SIZE * (i / CHARA_PER_LINE);
		
		CharacterSelectInfoPtr ptr(new CharacterSelectInfo (v[i], ImgManager::GetImage(string("Actors/") + v[i] + string("/icon.png")), NULL, pos));
		CharactersInfo& ci = *(m_characters.get());
		ci[i].swap(ptr);
		
		#ifdef _DEBUG
		LOG("Character loaded")
		#endif
	}
	
	//texts
	//m_title.SetFont(GameSystem::GetBattleFont());
	//m_title = "Character Select";
	//m_title.SetPosition(240 - m_title.GetStringWidth() / 2, 5);
	
	
	//variables
	m_current_actor		= 0																					;
	(m_chara_nb > CHARA_PER_LINE) ? m_current_enemy = CHARA_PER_LINE - 1 : m_current_enemy = m_chara_nb - 1	;
	m_current_bgm		= 0																					;
	m_can_plug_in		= false																				;
}

void MMBNCharacterSelect::Destroy()
{
	SndManager::StopBGM(0);
	/*for(unsigned int i(0) ; i < m_characters.size() ; ++i)
	{
		if(m_characters[i]) delete m_characters[i];
	}*/
	
	//if(m_characters) delete[] m_characters;
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
	
	//title
	MMBNString title;
	title.SetFont(GameSystem::GetBattleFont());
	title = "Character Select";
	title.SetPosition(240 - title.GetStringWidth() / 2, 5);
	title.Display();
	
	//characters
	CharactersInfo& ci = *(m_characters.get());
	
	for(unsigned int i(0) ; i < m_chara_nb ; ++i)
	{
		CharacterSelectInfoPtr& csi = ci[i];
		//frame
		oslDrawImageXY(m_chara_frame, csi->GetPosition().x, csi->GetPosition().y);
		//icon
		oslDrawImageXY(csi->GetIcon(), csi->GetPosition().x + 3, csi->GetPosition().y + 3);
	}
	
	//cursors
	//actor
	CharacterSelectInfoPtr& csi_a = ci[m_current_actor];
	m_actor_cursor->SetPosition(csi_a->GetPosition().x + CHARA_ICON_SIZE / 2, csi_a->GetPosition().y + CHARA_ICON_SIZE / 2);
	m_actor_cursor->Update();
	m_actor_cursor->Display();
	//enemy
	CharacterSelectInfoPtr& csi_e = ci[m_current_enemy];
	m_enemy_cursor->SetPosition(csi_e->GetPosition().x + CHARA_ICON_SIZE / 2, csi_e->GetPosition().y + CHARA_ICON_SIZE / 2);
	m_enemy_cursor->Update();
	m_enemy_cursor->Display();
	
	SndManager::PlayBGM("Digital_Strider.bgm", 0, true);

}