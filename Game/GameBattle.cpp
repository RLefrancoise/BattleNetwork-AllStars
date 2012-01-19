#include "GameBattle.h"

using namespace std;

GameBattle::BattleInfo GameBattle::BATTLE_INFO;

GameBattle::BattleInfo& GameBattle::GetBattleInfo()
{
	return BATTLE_INFO;
}

void GameBattle::SetBattleInfo(BattleInfo& info)
{
	BATTLE_INFO.bgName = info.bgName;
	
	BATTLE_INFO.actor.name = info.actor.name;
	BATTLE_INFO.actor.position.x = info.actor.position.x;
	BATTLE_INFO.actor.position.y = info.actor.position.y;
	
	BATTLE_INFO.enemies.clear();
	
	for(unsigned int i(0) ; i < info.enemies.size() ; ++i)
	{
		BattleCharacter c;
		c.name = info.enemies[i].name;
		c.position.x = info.enemies[i].position.x;
		c.position.y = info.enemies[i].position.y;
		BATTLE_INFO.enemies.push_back(c);
	}
	
	BATTLE_INFO.bgmName = info.bgmName;
}





		