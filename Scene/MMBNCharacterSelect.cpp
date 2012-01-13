#include "MMBNCharacterSelect.h"
#include "GameSystem.h"
#include "GameBattle.h"

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
	m_enemy_cursor		= Animation::Load("System/Menus/CharacterSelect/EnemyCursor");
		
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
		
		CharacterSelectInfoPtr ptr(new CharacterSelectInfo (v[i], ImgManager::GetImage(string("Actors/") + v[i] + string("/icon.png")), ImgManager::GetImage(string("Actors/") + v[i] + string("/body.png")), pos));
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
	m_current_actor			= 0																										;
	(m_chara_nb > CHARA_PER_LINE) 			? m_current_enemy 	= CHARA_PER_LINE - 1 	: m_current_enemy 	= m_chara_nb - 1		;
	(GameSystem::GetBGMNames().size() > 0) 	? m_current_bgm 	= 0 					: m_current_bgm 	= -1					;	
	(GameSystem::GetBackgroundsNames().size() > 0) 	? m_current_background 	= 0 		: m_current_background 	= -1				;
	
	m_can_plug_in			= false																									;
	
	m_is_selecting_actor 	= true																									;
	m_is_selecting_enemy 	= false																									;
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
		
		//L trigger pressed
		if(k->pressed.L)
		{
			m_current_bgm--;
			if(m_current_bgm < -1)
				m_current_bgm = GameSystem::GetBGMNames().size() - 1;
		}
		//R trigger pressed
		else if(k->pressed.R)
		{
			m_current_bgm++;
			if(m_current_bgm >= GameSystem::GetBGMNames().size())
				m_current_bgm = -1;
		}
		
		//square
		if(k->pressed.square)
		{
			m_current_background++;
			if(m_current_background >= GameSystem::GetBackgroundsNames().size())
				m_current_background = -1;
		}
		
		//cross
		if(k->pressed.cross)
		{
			//is selecting actor
			if(m_is_selecting_actor && !m_is_selecting_enemy)
			{
				m_is_selecting_actor = false;
				m_is_selecting_enemy = true;
			}
			//is selecting enemy
			else if(!m_is_selecting_actor && m_is_selecting_enemy)
			{
				m_is_selecting_enemy = false;
			}
			//has selected actor & enemy
			else if(!m_is_selecting_actor && ! m_is_selecting_enemy)
			{
				CharactersInfo& ci = *(m_characters.get());
				CharacterSelectInfoPtr& csi_a = ci[m_current_actor];
				CharacterSelectInfoPtr& csi_e = ci[m_current_enemy];
	
				//set battle info
				//actor
				GameBattle::BattleCharacter 				actor 		= { csi_a->GetName()		, Vector2i(2, 2) 		}	;
				//enemy
				GameBattle::BattleCharacter 				enemy 		= { csi_e->GetName()		, Vector2i(7, 2) 		}	;
				
				std::vector<GameBattle::BattleCharacter> 	v																	;
				v.push_back(enemy)																								;
				
				//bgm
				string bgm;
				(m_current_bgm != -1) ? bgm = GameSystem::GetBGMNames()[m_current_bgm].second : bgm = ""						;
				
				//background
				string bck;
				(m_current_background != -1) ? bck = GameSystem::GetBackgroundsNames()[m_current_background].second : bck = ""	;
				
				GameBattle::BattleInfo 						battle_info = {bck, actor	, v	, bgm		}						;
				GameBattle::SetBattleInfo(battle_info)																			;
				
				m_can_plug_in = true;
				
			}
		}
		
		//moving cursor
		if(m_is_selecting_actor) MoveCursor(k, &m_current_actor);
		else if(m_is_selecting_enemy) MoveCursor(k, &m_current_enemy);
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
	
	CharactersInfo& ci = *(m_characters.get());
	
	
	//background
	m_bg->Update();
	m_bg->Display();
	
	//bodies
	CharacterSelectInfoPtr& csi_a = ci[m_current_actor];
	CharacterSelectInfoPtr& csi_e = ci[m_current_enemy];
	//---current character body---
	if(csi_a->GetBody())
	{
		//body
		oslDrawImageXY(csi_a->GetBody(), 0, 272 - csi_a->GetBody()->sizeY);
		//name
		MMBNString s;
		s.SetFont(GameSystem::GetBattleFont());
		s = csi_a->GetName();
		s.SetPosition(75 - (s.GetStringWidth() / 2), 260 - s.GetStringHeight() / 2);
		s.Display();
	}
	//---current enemy body---
	if(csi_e->GetBody())
	{	
		//body
		oslMirrorImageH(csi_e->GetBody());
		oslDrawImageXY(csi_e->GetBody(), 480 - csi_e->GetBody()->sizeX, 272 - csi_a->GetBody()->sizeY);
		oslMirrorImageH(csi_e->GetBody());
		//name
		MMBNString s;
		s.SetFont(GameSystem::GetBattleFont());
		s = csi_e->GetName();
		s.SetPosition(405 - (s.GetStringWidth() / 2), 260 - s.GetStringHeight() / 2);
		s.Display();
	}
	
	//title
	MMBNString title;
	title.SetFont(GameSystem::GetBattleFont());
	title = "Character Select";
	title.SetPosition(240 - title.GetStringWidth() / 2, 35);
	title.Display();
	
	//characters
	for(unsigned int i(0) ; i < m_chara_nb ; ++i)
	{
		CharacterSelectInfoPtr& csi = ci[i];
		//frame
		oslDrawImageXY(m_chara_frame, csi->GetPosition().x, csi->GetPosition().y);
		//icon
		oslDrawImageXY(csi->GetIcon(), csi->GetPosition().x + 3, csi->GetPosition().y + 3);
	}
	
	//cursors
	//---actor---
	m_actor_cursor->SetPosition(csi_a->GetPosition().x + CHARA_ICON_SIZE / 2, csi_a->GetPosition().y + CHARA_ICON_SIZE / 2);
	m_actor_cursor->Update();
	m_actor_cursor->Display();
	//---enemy---
	m_enemy_cursor->SetPosition(csi_e->GetPosition().x + CHARA_ICON_SIZE / 2, csi_e->GetPosition().y + CHARA_ICON_SIZE / 2);
	m_enemy_cursor->Update();
	m_enemy_cursor->Display();
	
	//bgm
	MMBNString s_bgm;
	s_bgm.SetFont(GameSystem::GetCustomWindowLetterFont());
	(m_current_bgm != -1) ? s_bgm = GameSystem::GetBGMNames()[m_current_bgm].first : s_bgm = "None";
	s_bgm.SetPosition(240 - s_bgm.GetStringWidth() / 2, 262 - s_bgm.GetStringHeight() / 2);
	s_bgm.Display();
	OSL_IMAGE* l_trigger = GameSystem::GetControllerImage(GameSystem::L_TRIGGER);
	OSL_IMAGE* r_trigger = GameSystem::GetControllerImage(GameSystem::R_TRIGGER);
	oslDrawImageXY(l_trigger, s_bgm.GetPosition().x - l_trigger->sizeX - 2, s_bgm.GetPosition().y + abs((int) (l_trigger->sizeY - s_bgm.GetStringHeight())) / 2);
	oslDrawImageXY(r_trigger, s_bgm.GetPosition().x + s_bgm.GetStringWidth() + 2, s_bgm.GetPosition().y + abs((int) (r_trigger->sizeY - s_bgm.GetStringHeight())) / 2);
	
	//background
	MMBNString s_background;
	s_background.SetFont(GameSystem::GetCustomWindowLetterFont());
	(m_current_background != -1) ? s_background = GameSystem::GetBackgroundsNames()[m_current_background].first : s_background = "None";
	s_background.SetPosition(240 - s_background.GetStringWidth() / 2, 5);
	s_background.Display();
	OSL_IMAGE* square = GameSystem::GetControllerImage(GameSystem::SQUARE);
	
	oslDrawImageXY(square, s_background.GetPosition().x - square->sizeX - 2, s_background.GetPosition().y + abs((int) (square->sizeY - s_background.GetStringHeight())) / 2);
	
	
	//wait for x press after selecting characters
	if(!m_is_selecting_actor && !m_is_selecting_enemy)
	{
		MMBNString s;
		s.SetFont(GameSystem::GetCustomWindowLetterFont());
		s = "Jack In!";
		s.SetPosition(475 - s.GetStringWidth(), 5);
		s.Display();
		OSL_IMAGE* cross = GameSystem::GetControllerImage(GameSystem::CROSS);
		oslDrawImageXY(cross, s.GetPosition().x -  cross->sizeX - 2, s.GetPosition().y + abs((int) (cross->sizeY - s.GetStringHeight())) / 2);
	}
	
	
	SndManager::PlayBGM("Digital_Strider.bgm", 0, true);

}




void MMBNCharacterSelect::MoveCursor(OSL_CONTROLLER* k, int* cursor)
{
	if(k->pressed.left)
	{
		((*cursor) > 0) ? (*cursor)-- : (*cursor) = 0;
	}
	else if(k->pressed.right)
	{
		((*cursor) < m_chara_nb - 1) ? (*cursor)++ : (*cursor) = m_chara_nb - 1;
	}
	else if(k->pressed.up)
	{
		(*cursor) -= CHARA_PER_LINE;
		if((*cursor) < 0) (*cursor) += CHARA_PER_LINE;
	}
	else if(k->pressed.down)
	{
		(*cursor) += CHARA_PER_LINE;
		if((*cursor) > m_chara_nb - 1) (*cursor) -= CHARA_PER_LINE;
	}
}