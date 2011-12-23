#ifndef ActorLifeBar_H
#define ActorLifeBar_H

#include <oslib/oslib.h>

class BattleActor;

class ActorLifeBar
{

	public:

		ActorLifeBar(BattleActor* actor)					;
		~ActorLifeBar()										;
		
		void Display(int x, int y)	;
		void SetExtended(bool b)							;

	private:
	
		BattleActor* m_actor									;
		OSL_IMAGE* m_life_pic									;
		OSL_IMAGE* m_life_pic_damage							;
		OSL_IMAGE* m_life_pic_critical							;
		bool m_extended										;

};

#endif //ActorLifeBar_H
