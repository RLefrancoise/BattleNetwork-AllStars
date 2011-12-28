#ifndef TITLE_H
#define TITLE_H

#include "Screens.hpp"
#include "Timer.h"

#include <oslib/oslib.h>
#include <pspthreadman.h>

enum TitleMenu
{
	NEW_GAME,
	LOAD_GAME
};

class Title : public Screen
{

	public:

		
		Title()								;
		~Title()							;
		virtual int	Run()					;
		void	Initialize()				;
		void	Destroy()					;
		
	private:
		
		//Mise à jour de la scène (appelée par Run)
		virtual int	Update()	;
		//Affichage de la scène (appelée par Run)
		virtual void Display()	;
		
		//background
		OSL_IMAGE* m_bg;						;
		OSL_IMAGE* m_char						;
		OSL_IMAGE* m_newGame					;
		OSL_IMAGE* m_loadGame					;
		OSL_IMAGE* m_cursor						;

		//sf::Music m_bgm							;
		
		Timer m_clock						;

		int m_currentAlpha						;
		
		TitleMenu m_menu						;


};

#endif //TITLE_H
