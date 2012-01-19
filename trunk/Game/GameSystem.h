#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <map>
#include <string>
#include <oslib/oslib.h>

#include "Utils.h"
#include "Animation.hpp"

class MMBNFont;

class GameSystem
{

	public:
		
		enum GameController{
			TRIANGLE,
			SQUARE,
			CIRCLE,
			CROSS,
			L_TRIGGER,
			R_TRIGGER,
			CONTROLLER_NB
		};
		
		enum ChipElement{
			FIRE_ELEMENT,
			WATER_ELEMENT, 
			THUNDER_ELEMENT, 
			GRASS_ELEMENT, 
			HEAL_ELEMENT, 
			SWORD_ELEMENT, 
			NONE_ELEMENT,
			CHIP_ELEMENT_NB
		};

		enum ChipType{
			NORMAL_CHIP, 
			MEGA_CHIP, 
			GIGA_CHIP, 
			DARK_CHIP,
			CHIP_TYPE_NB
		};

		enum DamageType{
			FIXED_DAMAGE, 
			CURRENT_HP_DAMAGE, 
			DIFF_HP_DAMAGE, 
			MAX_HP_DAMAGE, 
			HEAL_DAMAGE, 
			NONE_DAMAGE					
		};

		enum TargetType{
			USER_TARGET,
			ENEMY_TARGET, 
			USER_AREA_TARGET, 
			ENEMY_AREA_TARGET,
			NONE_TARGET
		};
		
		
		
		//BATTLE
		typedef enum
		{
			NORMAL,
			EMPTY,
			BROKEN,
			CRACKED,
			FIRE,
			ICE,
			GRASS,
			POISON,
			WATER,
			PANEL_TYPES_NB
		} PanelType;
		
		typedef enum
		{
			PLAYER,
			ENEMY,
			PANELS_TEAM_NB
		} PanelTeam;
		
		struct AttackInfo
		{
			TargetType target_type;
			std::vector<Vector2i> range;
			std::vector<PanelTeam> target_teams;
			bool pierce_attack;
		};
		
		
		
		static void Initialize();
		static void Destroy();
		
		//========================
		// PICTURES
		//========================
		static OSL_IMAGE* GetControllerImage(GameController controller);
		static OSL_IMAGE* GetChipImage(ChipType type);
		static OSL_IMAGE* GetElementImage(ChipElement element);

		//========================
		// FONTS
		//========================
		static MMBNFont* GetBattleFont();
		static MMBNFont* GetActorLifeFont();
		static MMBNFont* GetBattleChipNameFont();
		static MMBNFont* GetBattleChipPowerFont();
		static MMBNFont* GetBattleChipLetterFont();
		static MMBNFont* GetBattleChipNormalDescFont();
		static MMBNFont* GetBattleEnemyNameFont();
		static MMBNFont* GetEnemyLifeFont();
		static MMBNFont* GetCustomWindowLetterFont();

		//========================
		// ANIMATIONS
		//========================
		static AnimationPtr GetLoadingAnimation();
		
		//========================
		// GAME DATA
		//========================
		static std::vector<std::string>& GetActorsNames();
		static std::vector<std::pair<std::string,std::string> >& GetBGMNames();
		static std::vector<std::pair<std::string,std::string> >& GetBackgroundsNames();
		
	private:
		
		GameSystem();
		
		//========================
		// PICTURES
		//========================
		static OSL_IMAGE* m_controller_pics[CONTROLLER_NB];
		static OSL_IMAGE* m_chip_pics[CHIP_TYPE_NB]		;
		static OSL_IMAGE* m_element_pics[CHIP_ELEMENT_NB];

		//========================
		// FONTS
		//========================
		static MMBNFont* m_actor_life_font;
		static MMBNFont* m_battle_chip_name_font;
		static MMBNFont* m_battle_chip_power_font;
		static MMBNFont* m_battle_chip_letter_font;
		static MMBNFont* m_battle_enemy_name_font;
		static MMBNFont* m_enemy_life_font;
		static MMBNFont* m_custom_window_letter_font;
		
		//========================
		// ANIMATIONS
		//========================
		static std::vector<AnimationPtr> LOADING_ANIMATIONS;
		
		//========================
		// GAME DATA
		//========================
		static std::vector<std::string> ACTORS_NAMES;
		static std::vector<std::pair<std::string,std::string> > BGM_NAMES;
		static std::vector<std::pair<std::string,std::string> > BACKGROUNDS_NAMES;
};

#endif
