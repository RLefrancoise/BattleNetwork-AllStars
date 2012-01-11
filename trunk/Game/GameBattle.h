#ifndef GAMEBATTLE_H
#define GAMEBATTLE_H

#include <string>

#include "Utils.h"

class GameBattle
{

	public:
		
		struct BattleCharacter
		{
			std::string name;
			Vector2i position;
		};
		
		struct BattleInfo
		{
			std::string bgName;
			BattleCharacter actor;
			std::vector<BattleCharacter> enemies;
			std::string bgmName;
		};
		
		static BattleInfo& 	GetBattleInfo();
		static void			SetBattleInfo(BattleInfo& info);
		
	private:
		GameBattle();
		
		static BattleInfo BATTLE_INFO;
		
};

#endif
