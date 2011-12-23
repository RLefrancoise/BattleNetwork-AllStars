#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "ExtendedSprite.h"
#include "Utils.h"

#include <vector>
#include <string>
#include <oslib/oslib.h>

class Animation
{
	public :

		Animation(std::string spritesPath, std::vector<int> delays, bool reverse, bool loop = true);
		~Animation();
		static Animation* Load(std::string spritesPath, bool reverse = false, bool loop = true);
		ExtendedSprite& GetCurrentSprite();
		void SetPosition(float x, float y);
		void Display(float offX = 0, float offY = 0);

		void Start();
		void Update();
		void Pause();
		void Stop();
		bool IsRunning();
		bool IsOver();

	private :

		static void InitDelaysOfAnim(std::string path, std::vector<int> &delays);

		std::vector<ExtendedSprite> m_sprites;
		Timer					m_clock;
		unsigned int			m_currentFrame;
		std::vector<int>		m_delays;
		bool					m_running;
		bool					m_loop;
		bool					m_isOver;
		float					m_currentTime;

};

#endif // ANIMATION_HPP