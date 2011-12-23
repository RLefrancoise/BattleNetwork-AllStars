#ifndef BATTLEACTOR_H
#define BATTLEACTOR_H

#include "Animation.hpp"
#include "Object.h"
#include "ActorLifeBar.h"
#include "Utils.h"

class BattleActor
{

	public:

		enum ActorDirection
		{
			LEFT,
			RIGHT,
			UP,
			DOWN,
			UP_LEFT,
			UP_RIGHT,
			DOWN_LEFT,
			DOWN_RIGHT
		};

		enum ActorState
		{
			BATTLE_STANDING_LEFT,
			BATTLE_WALKING_LEFT,
			BATTLE_STANDING_RIGHT,
			BATTLE_WALKING_RIGHT,
		};

		virtual ~BattleActor()									;
		
		static BattleActor* Load(std::string name)				;
		void Display(float offX = 0, float offY = 0)		;

		bool Collide(Shape& s);

		void Move(float x, float y)								;
		//===============================
		// SETTERS
		//===============================
		void SetDirection(ActorDirection direction)			;
		void SetState(ActorState state)						;
		void SetPosition(float x, float y)						;
		//===============================
		// GETTERS
		//===============================
		Shape& GetColliShape()								;
		ActorDirection GetDirection() const					;
		ActorState GetState() const							;
		ActorLifeBar* GetLifeBar() const					;
		std::string GetName() const							;
		Animation* GetCurrentAnim()							;
		Vector2f GetPosition()								;
		Vector2f GetOffset()								;
		Vector2f GetPositionWithOffset()					;
		int GetLV() const									;
		int GetHP() const									;
		int GetMaxHP() const								;
		int GetStr() const									;
		int GetDef() const									;
		int GetInt() const									;
		int GetXVel() const									;
		int GetYVel() const									;
		int GetActorSurface() const							;

	protected:

		int IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P);

		//===============================
		// CONSTRUCTEUR
		//===============================
		BattleActor()								;
		BattleActor(std::string)					;

		//===============================
		// FONCTIONS PRIVEES
		//===============================
		void InitDelaysOfAnim(std::string path, std::vector<int> &delays);
		void LoadStateAnim(ActorState state);

		//===============================
		// LIFE BAR
		//===============================
		ActorLifeBar* m_lifeBar				;

		//===============================
		// ETAT & DIRECTION
		//===============================
		ActorDirection m_direction			;
		ActorState m_state					;

		//===============================
		// ANIMATIONS
		//===============================
		std::map<ActorState,Animation*> m_animMap	;
		std::map<ActorState,Animation*> m_reversedAnimMap	;
		float m_posX;
		float m_posY;
		float m_offX;
		float m_offY;
		Shape m_colliDiamond;
		Shape m_colliRect;

		int m_actorSurface;
		//===============================
		// STATISTIQUES
		//===============================
		std::string m_name					;
		int m_lv							;
		int m_hp							;
		int m_maxHp							;
		int m_str							;
		int m_def							;
		int m_int							;
		int m_xVel							;
		int m_yVel							;

};

#endif //BATTLEACTOR_H
