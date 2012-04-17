#ifndef MMBN_ATTACKINFO_H
#define MMBN_ATTACKINFO_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <boost/shared_ptr.hpp>

#include "Animation.hpp"
#include "GameSystem.h"

class AttackInfo
{
	private:
		GameSystem::TargetType target_type;
		std::vector<Vector2i> range;
		std::vector<GameSystem::PanelTeam> target_teams;
		bool pierce_attack;
		bool stagger_enemy;
		std::vector<int> hit_frames;

		
	public:
		AttackInfo();
		AttackInfo(std::string file);
		~AttackInfo();
		
		void SetTargetType(GameSystem::TargetType target);
		void SetRange(std::vector<Vector2i> range);
		void SetTeams(std::vector<GameSystem::PanelTeam> teams);
		void SetPierceAttack(bool pierce);
		void SetStaggerEnemy(bool stagger);
		void SetHitFrames(std::vector<int> frames);
		
		GameSystem::TargetType GetTargetType() const;
		std::vector<Vector2i> GetRange() const;
		std::vector<GameSystem::PanelTeam> GetTeams() const;
		bool IsPierceAttack();
		bool IsStaggerAttack();
		std::vector<int>& GetHitFrames();
		
		std::string ToString();
};

typedef boost::shared_ptr<AttackInfo> AttackInfoPtr				;

class BattleProjectile
{
	private:
		std::string name;
		AnimationPtr animation;
		unsigned int velocity;
		Vector2i position;
		std::vector<unsigned int> hitting_frames;
		unsigned int damage;
		
	public:
		BattleProjectile();
		~BattleProjectile();
};
		
class MMBNBattleAttack
{
	private:
		std::string name;
		unsigned int power;
		AttackInfoPtr attack_info;
		bool use_projectile;
		unsigned int projectiles_number;
		std::map<unsigned int, std::vector<BattleProjectile> > projectiles_map;
		//std::string actor_animation_name;
		AnimationPtr actor_animation;
		
	public:
		MMBNBattleAttack();
		MMBNBattleAttack(std::string file, std::string attack_info_file);
		~MMBNBattleAttack();
		
		std::string GetName();
		unsigned int GetPower();
		AttackInfoPtr& GetAttackInfo();
		bool UseProjectile();
		unsigned int GetProjectilesNumber();
		std::map<unsigned int, std::vector<BattleProjectile> > GetProjectilesMap() const;
		AnimationPtr GetAnimation();
		
		std::string ToString();
};

typedef boost::shared_ptr<MMBNBattleAttack> MMBNBattleAttackPtr		;

#endif
