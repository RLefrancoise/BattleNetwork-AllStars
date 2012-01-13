#include "GameSystem.h"

#include <sstream>

using namespace std;

OSL_IMAGE* GameSystem::m_controller_pics[CONTROLLER_NB];
OSL_IMAGE* GameSystem::m_chip_pics[CHIP_TYPE_NB];
OSL_IMAGE* GameSystem::m_element_pics[CHIP_ELEMENT_NB];


MMBNFont* GameSystem::m_actor_life_font;
MMBNFont* GameSystem::m_battle_chip_name_font;
MMBNFont* GameSystem::m_battle_chip_power_font;
MMBNFont* GameSystem::m_battle_chip_letter_font;
MMBNFont* GameSystem::m_battle_enemy_name_font;
MMBNFont* GameSystem::m_enemy_life_font;
MMBNFont* GameSystem::m_custom_window_letter_font;

vector<AnimationPtr> GameSystem::LOADING_ANIMATIONS;

vector<string> 					GameSystem::ACTORS_NAMES	;
vector<pair<string,string> > 	GameSystem::BGM_NAMES		;
vector<pair<string,string> > 	GameSystem::BACKGROUNDS_NAMES;

void GameSystem::Initialize()
{
	//========================
	// PICTURES
	//========================
	//CONTROLLER
	m_controller_pics[TRIANGLE]	= ImgManager::GetImage("System/Menus/triangle.png")						;
	m_controller_pics[SQUARE]	= ImgManager::GetImage("System/Menus/square.png")						;
	m_controller_pics[CIRCLE]	= ImgManager::GetImage("System/Menus/circle.png")						;
	m_controller_pics[CROSS]	= ImgManager::GetImage("System/Menus/cross.png")						;
	m_controller_pics[L_TRIGGER]= ImgManager::GetImage("System/Menus/L.png")							;
	m_controller_pics[R_TRIGGER]= ImgManager::GetImage("System/Menus/R.png")							;
	
	//CHIPS
	m_chip_pics[NORMAL_CHIP]	= ImgManager::GetImage("System/Chips/CustomWindow/normal.png")			;
	m_chip_pics[MEGA_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/mega.png")			;
	m_chip_pics[GIGA_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/giga.png")			;
	m_chip_pics[DARK_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/dark.png")			;
	
	//========================
	// ANIMATIONS
	//========================
	//LOADING ANIMATIONS
	{
		ifstream in( "System/Animation/Loading/animations.txt" , ifstream::in);
		
		if(!in.good())
		{
			LOG("Impossible de trouver le fichier System/Animation/Loading/animations.txt");
			oslQuit();
		}
		
		string line;
		while(getline(in, line))
		{
			vector<string> v = StringUtils::Split(line, ":");
			LOADING_ANIMATIONS.push_back(Animation::Load(string("System/Animation/Loading/") + v[0]));
		}
		
		in.close();
	}
	//========================
	// GAME DATA
	//========================
	//ACTORS NAMES
	{
		ifstream in( "Actors/actors.txt" , ifstream::in);
		
		if(!in.good())
		{
			LOG("Impossible de trouver le fichier Actors/actors.txt");
			oslQuit();
		}
		
		string line;
		while(getline(in, line))
		{
			vector<string> v = StringUtils::Split(line, ":");
			ACTORS_NAMES.push_back(v[0]);
		}
		
		in.close();
	}
	
	//BGM NAMES
	{
		ifstream in( "Audio/BGM/bgm.txt" , ifstream::in);
		
		if(!in.good())
		{
			LOG("Impossible de trouver le fichier Audio/BGM/bgm.txt");
			oslQuit();
		}
		
		string line;
		while(getline(in, line))
		{
			vector<string> v = StringUtils::Split(line, ":");
			vector<string> w = StringUtils::Split(v[0], "|");
			BGM_NAMES.push_back(pair<string,string>(w[0], w[1]));
		}
		
		in.close();
	}
	
	//BACKGROUNDS NAMES
	{
		ifstream in( "Backgrounds/backgrounds.txt" , ifstream::in);
		
		if(!in.good())
		{
			LOG("Impossible de trouver le fichier Backgrounds/backgrounds.txt");
			oslQuit();
		}
		
		string line;
		while(getline(in, line))
		{
			vector<string> v = StringUtils::Split(line, ":");
			vector<string> w = StringUtils::Split(v[0], "|");
			BACKGROUNDS_NAMES.push_back(pair<string,string>(w[0], w[1]));
		}
		
		in.close();
	}
}

void GameSystem::Destroy()
{
	
}


OSL_IMAGE* GameSystem::GetControllerImage(GameController controller)
{
	return m_controller_pics[controller];
}

OSL_IMAGE* GameSystem::GetChipImage(ChipType type)
{
	return m_chip_pics[type];
}

OSL_IMAGE* GameSystem::GetElementImage(ChipElement element)
{
	return m_element_pics[element];
}

MMBNFont* GameSystem::GetBattleFont()
{
	return FontManager::GetFont("MMBNBattleFont");
}

MMBNFont* GameSystem::GetActorLifeFont()
{
	return FontManager::GetFont("MMBNActorLifeFont");
}

MMBNFont* GameSystem::GetBattleChipNameFont()
{
	return FontManager::GetFont("MMBNBattleFont");
}

MMBNFont* GameSystem::GetBattleChipPowerFont()
{
	return FontManager::GetFont("MMBNBattleChipPowerFont");
}

MMBNFont* GameSystem::GetBattleChipLetterFont()
{
	return FontManager::GetFont("MMBNBattleChipLetterFont");
}

MMBNFont* GameSystem::GetBattleChipNormalDescFont()
{
	return FontManager::GetFont("MMBNBattleChipNormalDescFont");
}

MMBNFont* GameSystem::GetBattleEnemyNameFont()
{
	return FontManager::GetFont("MMBNBattleFont");
}

MMBNFont* GameSystem::GetEnemyLifeFont()
{
	return FontManager::GetFont("MMBNEnemyLifeFont");
}

MMBNFont* GameSystem::GetCustomWindowLetterFont()
{
	return FontManager::GetFont("MMBNCustomWindowLetterFont");
}
		
		
AnimationPtr GameSystem::GetLoadingAnimation()
{
	return LOADING_ANIMATIONS[Random::RandomInt(0, LOADING_ANIMATIONS.size())];
}




vector<string>& GameSystem::GetActorsNames()
{
	return ACTORS_NAMES;
}

vector<pair<string,string> >& GameSystem::GetBGMNames()
{
	return BGM_NAMES;
}

vector<pair<string,string> >& GameSystem::GetBackgroundsNames()
{
	return BACKGROUNDS_NAMES;
}