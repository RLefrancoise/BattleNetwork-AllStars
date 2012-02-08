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
			bool stagger_enemy;
			std::vector<int> hit_frames;
		};
		
		struct BattleProjectile
		{
			std::string name;
			AnimationPtr animation;
			unsigned int velocity;
			Vector2i position;
			std::vector<unsigned int> hitting_frames;
			unsigned int damage;
		};
		
		struct BattleAttack
		{
			std::string name;
			unsigned int power;
			AttackInfo attack_info;
			bool use_projectile;
			unsigned int projectiles_number;
			std::map<unsigned int, std::vector<BattleProjectile> > projectiles_map;
			std::string actor_animation_name;
		};
		
		static void Initialize();
		static void Destroy();
		
		//========================
		// STRUCTURES LOADING
		//========================
		static GameSystem::ChipElement 	GetElementOfString(std::string e)		;
		static GameSystem::DamageType 	GetDamageOfString(std::string d)		;
		static GameSystem::TargetType 	GetTargetOfString(std::string t)		;
		static GameSystem::ChipType 	GetChipTypeOfString(std::string c)		;
		
		static GameSystem::PanelTeam 	GetPanelTeamOfString(std::string p)		;
		
		static void InitAttackInfo(std::string file, AttackInfo* ai)			;
		static void InitBattleAttack(std::string file, BattleAttack* ba)		;
		
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
		
		static std::map<std::string, GameSystem::ChipElement> 	ELEMENT_STRING_MAP		;
		static std::map<std::string, GameSystem::DamageType> 	DAMAGE_STRING_MAP		;
		static std::map<std::string, GameSystem::TargetType> 	TARGET_STRING_MAP		;
		static std::map<std::string, GameSystem::ChipType> 		CHIPTYPE_STRING_MAP		;
		
		static std::map<std::string, GameSystem::PanelTeam> 	PANELTEAM_STRING_MAP	;
		
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
