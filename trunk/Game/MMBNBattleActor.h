#ifndef MMBNBATTLEACTOR_H
#define MMBNBATTLEACTOR_H

#include "Displayable.h"
#include "Animation.hpp"
#include "Utils.h"

class MMBNBattleActor : public Displayable
{

	public:

		typedef struct
		{
			unsigned int moving_time;
		} IAConfig;

		enum ActorDirection
		{
			LEFT,
			RIGHT,
		};

		enum ActorState
		{
			BATTLE_STANDING,
			BATTLE_ATTACK,
			BATTLE_DAMAGED,
			BATTLE_DEAD
		};

		virtual ~MMBNBattleActor()									;
		
		static MMBNBattleActor* Load(std::string name, bool ia = true);
		void Display(float offX = 0, float offY = 0)		;

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
		Vector2f& GetPosition()								;
		int GetLV() const									;
		int GetHP() const									;
		int GetMaxHP() const								;
		int GetAtk() const									;
		int GetChrg() const									;
		int GetSpd() const									;

		void Attack(MMBNBattleActor* mmbnba)				;
		bool IsDead()										;

		IAConfig* GetIAConfig()								;

	protected:

		//===============================
		// CONSTRUCTEUR
		//===============================
		MMBNBattleActor()								;
		MMBNBattleActor(std::string)					;

		//===============================
		// FONCTIONS PRIVEES
		//===============================
		void InitDelaysOfAnim(std::string path, std::vector<int> &delays);
		void LoadStateAnim(ActorState state);

		void InitializeIA();

		//===============================
		// ETAT & DIRECTION
		//===============================
		ActorDirection m_direction			;
		ActorState m_state					;

		bool	m_displayLife				;

		//===============================
		// ANIMATIONS
		//===============================
		std::map<ActorState,Animation*> m_animMap	;
		std::map<ActorState,Animation*> m_reversedAnimMap	;
		Vector2f m_position;
		float m_posX;
		float m_posY;
		float m_offX;
		float m_offY;

		//===============================
		// STATISTIQUES
		//===============================
		std::string m_name					;
		int m_lv							;
		int m_hp							;
		int m_maxHp							;
		int m_atk							;
		int m_chrg							;
		int m_spd							;

		IAConfig m_ia_config				;

};

#endif //MMBNBATTLEACTOR_H
