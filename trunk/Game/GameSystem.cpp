#include "GameSystem.h"

#include <sstream>

using namespace std;

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


void GameSystem::Initialize()
{
	//PICTURES
	m_chip_pics[NORMAL_CHIP]	= ImgManager::GetImage("System/Chips/CustomWindow/normal.png")			;
	m_chip_pics[MEGA_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/mega.png")			;
	m_chip_pics[GIGA_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/giga.png")			;
	m_chip_pics[DARK_CHIP]		= ImgManager::GetImage("System/Chips/CustomWindow/dark.png")			;
	
	
	//LOADING ANIMATIONS
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

void GameSystem::Destroy()
{
	
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