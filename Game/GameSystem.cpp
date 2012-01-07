#include "GameSystem.h"

OSL_IMAGE* GameSystem::m_chip_pics[CHIP_TYPE_NB];
OSL_IMAGE* GameSystem::m_element_pics[CHIP_ELEMENT_NB];
MMBNFont* GameSystem::m_actor_life_font;
MMBNFont* GameSystem::m_battle_chip_name_font;
MMBNFont* GameSystem::m_battle_chip_power_font;
MMBNFont* GameSystem::m_battle_chip_letter_font;
MMBNFont* GameSystem::m_battle_enemy_name_font;
MMBNFont* GameSystem::m_enemy_life_font;
MMBNFont* GameSystem::m_custom_window_letter_font;


void GameSystem::Initialize()
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
		