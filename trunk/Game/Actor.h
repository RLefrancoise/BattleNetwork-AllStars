#ifndef ACTOR_H
#define ACTOR_H

#include "FieldMapElement.h"
#include "Animation.hpp"
//#include "Object.h"
#include "Utils.h"

class Actor : public FieldMapElement
{

	public:

		enum ActorProperties
		{
			WALKABLE,
			TURNABLE,
			PROPERTIES_NB
		};

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
			FIELD_STANDING_UP,
			FIELD_RUN_UP,
			FIELD_STANDING_DOWN,
			FIELD_RUN_DOWN,
			FIELD_STANDING_LEFT,
			FIELD_RUN_LEFT,
			FIELD_STANDING_RIGHT,
			FIELD_RUN_RIGHT,
			FIELD_STANDING_UP_LEFT,
			FIELD_RUN_UP_LEFT,
			FIELD_STANDING_UP_RIGHT,
			FIELD_RUN_UP_RIGHT,
			FIELD_STANDING_DOWN_LEFT,
			FIELD_RUN_DOWN_LEFT,
			FIELD_STANDING_DOWN_RIGHT,
			FIELD_RUN_DOWN_RIGHT
		};

		virtual ~Actor()									;
		
		static Actor* Load(std::string name, bool* properties = NULL);
		void Display(float offX = 0, float offY = 0)		;

		bool Collide(Collidable* o)							;

		void Move(float x, float y)								;
		//===============================
		// SETTERS
		//===============================
		void SetDirection(ActorDirection direction)			;
		void SetState(ActorState state)						;
		void SetPosition(float x, float y)					;
		//===============================
		// GETTERS
		//===============================
		Shape& GetColliShape()								;
		ActorDirection GetDirection() const					;
		ActorState GetState() const							;
		std::string GetName() const							;
		Animation* GetCurrentAnim()							;
		const Vector2f& GetPosition() const					;
		Vector2f& GetTilePosition()							;
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

		//int IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P);
		void LoadStates();
		//===============================
		// CONSTRUCTEUR
		//===============================
		Actor()								;
		Actor(std::string)					;

		//===============================
		// FONCTIONS PRIVEES
		//===============================
		void InitDelaysOfAnim(std::string path, std::vector<int> &delays);
		void LoadStateAnim(ActorState state);

		//===============================
		// ETAT & DIRECTION
		//===============================
		ActorDirection m_direction			;
		ActorState m_state					;
		bool m_properties[PROPERTIES_NB]	;
		//===============================
		// ANIMATIONS
		//===============================
		std::map<ActorState,Animation*> m_animMap	;
		std::map<ActorState,Animation*> m_reversedAnimMap	;
		float m_posX;
		float m_posY;
		Vector2f m_pos;
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

#endif //ACTOR_H
