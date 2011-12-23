#include "MMBNBattleChip.h"
#include "MMBNString.h"
#include "FontManager.h"

#include <sstream>

using namespace std;

map<string, GameSystem::ChipElement>	MMBNBattleChip::m_element_map		;
map<string, GameSystem::DamageType>		MMBNBattleChip::m_damage_map		;
map<string, GameSystem::TargetType>		MMBNBattleChip::m_target_map		;
map<string, GameSystem::ChipType>		MMBNBattleChip::m_type_map			;

MMBNBattleChip::MMBNBattleChip()
{
	m_name				= ""							;
	m_description		= ""							;
	m_element			= GameSystem::NONE_ELEMENT		;
	m_power				= 0								;
	m_letter			= '*'							;
	m_pic				= NULL							;
	m_icon				= NULL							;
	m_damage_type		= GameSystem::NONE_DAMAGE		;
	m_chip_type			= GameSystem::NORMAL_CHIP		;
	m_target_type		= GameSystem::NONE_TARGET		;
	m_pause_when_used	= false							;

	m_extended_display = true							;
}

MMBNBattleChip::~MMBNBattleChip()
{

}

MMBNBattleChip* MMBNBattleChip::Load(std::string name)
{
	MMBNBattleChip* c = new MMBNBattleChip();

	ifstream in_chip( (string("Battle/Chips/") + name + string("/info.txt")).c_str() , ifstream::in);

	if(!in_chip.good())
	{
		LOG("Impossible de trouver le fichier info.txt pour " + string("Battle/Chips/") + name);
		oslQuit();
	}

	string line;
	while(getline(in_chip, line))
	{
		vector<string> v = StringUtils::Split(line, ":");

		//NAME
		if(line.find("name") == 0)
			c->m_name = v.at(1);

		//DESC
		else if(line.find("desc") == 0)
			c->m_description = v.at(1);
		
		//TYPE
		else if(line.find("type") == 0)
			c->m_chip_type = m_type_map[v.at(1)];
			
		//ELEMENT
		else if(line.find("element") == 0)
			c->m_element = m_element_map[v.at(1)];

		//POWER
		else if(line.find("power") == 0)
		{
			istringstream iss(v.at(1));
			iss >> c->m_power;
		}

		//LETTER
		else if(line.find("letter") == 0)
			c->m_letter = v.at(1).at(0);
		
		//DAMAGE
		else if(line.find("damage") == 0)
			c->m_damage_type = m_damage_map[v.at(1)];
		
		//TARGET
		else if(line.find("target") == 0)
			c->m_target_type = m_target_map[v.at(1)];

		//PAUSE
		else if(line.find("pause") == 0)
		{
			if(v.at(1).find("true") == 0)
				c->m_pause_when_used = true;
			else
				c->m_pause_when_used = false;
		}
		
	}

	in_chip.close();

	c->m_icon = ImgManager::GetImage( string("Battle/Chips/") + name + string("/icon.png") );
	c->m_pic  = ImgManager::GetImage( string("Battle/Chips/") + name + string("/pic.png") );

	return c;
}

void MMBNBattleChip::Display(float offX, float offY)
{
	m_extended_display ? DisplayExtended(offX, offY) : DisplaySimple(offX, offY);
}

void MMBNBattleChip::Move(float x, float y)
{
	m_position.x += x;
	m_position.y += y;
}

void MMBNBattleChip::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNBattleChip::GetPosition()
{
	return m_position;
}

OSL_IMAGE* MMBNBattleChip::GetImage()
{
	return m_pic;
}

OSL_IMAGE* MMBNBattleChip::GetIcon()
{
	return m_icon;
}

char MMBNBattleChip::GetLetter()
{
	return m_letter;
}

GameSystem::ChipType MMBNBattleChip::GetType()
{
	return m_chip_type;
}

void MMBNBattleChip::SetExtendedDisplay(bool extended)
{
	m_extended_display = extended;
}

void MMBNBattleChip::LoadMaps()
{
	//ELEMENTS
	m_element_map["fire"]		= GameSystem::FIRE_ELEMENT		;
	m_element_map["water"]		= GameSystem::WATER_ELEMENT		;
	m_element_map["thunder"]	= GameSystem::THUNDER_ELEMENT	;
	m_element_map["grass"]		= GameSystem::GRASS_ELEMENT		;
	m_element_map["heal"]		= GameSystem::HEAL_ELEMENT		;
	m_element_map["sword"]		= GameSystem::SWORD_ELEMENT		;
	m_element_map["none"]		= GameSystem::NONE_ELEMENT		;

	//DAMAGE
	m_damage_map["fixed"]		= GameSystem::FIXED_DAMAGE		;
	m_damage_map["current_hp"]	= GameSystem::CURRENT_HP_DAMAGE	;
	m_damage_map["diff_hp"]		= GameSystem::DIFF_HP_DAMAGE	;
	m_damage_map["max_hp"]		= GameSystem::MAX_HP_DAMAGE		;
	m_damage_map["heal"]		= GameSystem::HEAL_DAMAGE		;
	m_damage_map["none"]		= GameSystem::NONE_DAMAGE		;

	//TARGET
	m_target_map["user"]		= GameSystem::USER_TARGET		;
	m_target_map["enemy"]		= GameSystem::ENEMY_TARGET		;
	m_target_map["user_area"]	= GameSystem::USER_AREA_TARGET	;
	m_target_map["enemy_area"]	= GameSystem::ENEMY_AREA_TARGET	;
	m_target_map["none"]		= GameSystem::NONE_TARGET		;

	//TYPE
	m_type_map["normal"]		= GameSystem::NORMAL_CHIP		;
	m_type_map["mega"]			= GameSystem::MEGA_CHIP			;
	m_type_map["giga"]			= GameSystem::GIGA_CHIP			;
	m_type_map["dark"]			= GameSystem::DARK_CHIP			;
}


void MMBNBattleChip::DisplayExtended(float offX, float offY)
{
	//chip type
	oslDrawImageXY(GameSystem::GetChipImage(m_chip_type), m_position.x + offX, m_position.y + offY);
	//chip picture
	oslDrawImageXY(m_pic, m_position.x + 13, m_position.y + 6);
	//chip elements
	oslDrawImageXY(GameSystem::GetElementImage(m_element), m_position.x + 27, m_position.y + 56);
	//chip power
	if( (m_power > 0) && (m_damage_type == GameSystem::FIXED_DAMAGE) )
	{
		MMBNString s;
		s.SetFont(GameSystem::GetBattleChipPowerFont());
		s = m_power;
		s.SetPosition(m_position.x + 72 - s.GetStringWidth(), m_position.y + 55);
		s.Display();
	}
	else if(	(m_damage_type == GameSystem::CURRENT_HP_DAMAGE)
			||  (m_damage_type == GameSystem::DIFF_HP_DAMAGE)
			||  (m_damage_type == GameSystem::MAX_HP_DAMAGE)
	)
	{
		MMBNString s;
		s.SetFont(GameSystem::GetBattleChipPowerFont());
		s = "??";
		s.SetPosition(m_position.x + 72 - s.GetStringWidth(), m_position.y + 55);
		s.Display();
	}
	//chip letter
	MMBNString letter;
	letter.SetFont(GameSystem::GetBattleChipLetterFont());
	letter = m_letter;
	letter.SetPosition(m_position.x + 10, m_position.y + 55);
	letter.Display();
}

void MMBNBattleChip::DisplaySimple(float offX, float offY)
{
	//name
	MMBNString name;
	name.SetFont(GameSystem::GetBattleChipNameFont());
	name = m_name;
	name.SetPosition( (m_pic->sizeX - name.GetStringWidth()) / 2 + m_position.x + offX , m_position.y + offY);
	name.Display();

	//picture
	oslDrawImageXY(m_pic, m_position.x + offX, m_position.y + offY + name.GetFont()->GetCharHeight() + 2);

	//letter
	MMBNString letter;
	letter.SetFont(GameSystem::GetBattleChipLetterFont());
	letter = m_letter;
	letter.SetPosition(m_position.x + offX, m_position.y + offY + name.GetFont()->GetCharHeight() + m_pic->sizeY + 6);
	letter.Display();

	//element

	//power
	MMBNString s;
	s.SetFont(GameSystem::GetBattleChipPowerFont());

	switch(m_damage_type)
	{
		case GameSystem::FIXED_DAMAGE:
			if( m_power > 0 )
			{
				s = m_power;
				s.SetPosition(m_position.x + offX + m_pic->sizeX - s.GetStringWidth(), m_position.y + offY + name.GetFont()->GetCharHeight() + m_pic->sizeY + 6);
				s.Display();
			}
			break;

		case GameSystem::CURRENT_HP_DAMAGE:
		case GameSystem::DIFF_HP_DAMAGE:
		case GameSystem::MAX_HP_DAMAGE:
			s = "???";
			s.SetPosition(m_position.x + offX + m_pic->sizeX - s.GetStringWidth(), m_position.y + offY + name.GetFont()->GetCharHeight() + m_pic->sizeY + 6);
			s.Display();
			break;

		case GameSystem::HEAL_DAMAGE:
		case GameSystem::NONE_DAMAGE:
			break;
			
	}
		
}
