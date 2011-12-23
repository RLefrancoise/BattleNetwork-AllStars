#ifndef EVENT_H
#define EVENT_H

#include "Animation.hpp"
#include <map>

class Event
{

	public:

		virtual ~Event()									;
		
		static Event* Load(std::string name)				;
		virtual void Display()			;

		virtual void Move(float x, float y)					;
		//===============================
		// SETTERS
		//===============================
		virtual void SetPosition(float x, float y)			;
		void SetCurrentAnim(std::string anim)				;
		//===============================
		// GETTERS
		//===============================
		std::string GetName() const							;
		Animation* GetCurrentAnim()							;
		//sf::Vector2f GetPosition()							;
		int GetLV() const									;
		int GetHP() const									;
		int GetMaxHP() const								;
		int GetStr() const									;
		int GetDef() const									;
		int GetInt() const									;
		int GetXVel() const									;
		int GetYVel() const									;
		
	protected:

		//===============================
		// CONSTRUCTEUR
		//===============================
		Event()								;
		Event(std::string)					;

		//===============================
		// FONCTIONS PRIVEES
		//===============================
		void InitDelaysOfAnim(std::string path, std::vector<int> &delays);
		void LoadAnim(std::string animPath);
		//===============================
		// ANIMATIONS
		//===============================
		std::map<std::string,Animation*> m_animMap	;
		float m_posX;
		float m_posY;
		std::string m_currentAnim;
		//===============================
		// STATISTIQUES
		//===============================
		std::string m_name					;


};

#endif //ACTOR_H
