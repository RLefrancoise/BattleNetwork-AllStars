#ifndef BATTLEMAP_H
#define BATTLEMAP_H

#include "Screens.hpp"
#include "BattleActor.h"
#include "Utils.h"
#include "Variables.h"

#include <boost/shared_array.hpp>
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

#include <oslib/oslib.h>

class BattleMap : public Screen
{

	public:

		enum ActorMoving
		{
			AM_LEFT,
			AM_RIGHT,
			AM_UP,
			AM_DOWN,
			AM_UP_LEFT,
			AM_UP_RIGHT,
			AM_DOWN_LEFT,
			AM_DOWN_RIGHT
		};

		//Construit la carte
		BattleMap()								;
		//D�truit la carte
		~BattleMap()							;
		//D�marre la carte
		virtual int	Run()		;

		void	Initialize()										;

	private:

		//Mise � jour de la sc�ne (appel�e par Run)
		virtual int	Update()	;
		//Affichage de la sc�ne (appel�e par Run)
		virtual void Display()	;

		//gestion des �v�nements en relation avec le h�ros
		void ActorHandle()	;

		void MoveActor(BattleActor* actor, ActorMoving move);

		//background
		OSL_IMAGE*	m_bg;						;
		//vue
		View	m_view							;

		OSL_SOUND* m_bgm						;

		//===========================
		// ACTOR
		//===========================
		BattleActor* m_actor; //acteur

		int m_actorLifeBar_x;
		int m_actorLifeBar_y;

		//===========================
		// ENEMY
		//===========================
		BattleActor* m_enemy; //ennemi

		//===========================
		// COLLISIONS
		//===========================
		std::vector<AABBCollisionBox> m_collisions;
		std::vector<AABBCollisionBox> m_objects;

};

#endif //BATTLEMAP_H
