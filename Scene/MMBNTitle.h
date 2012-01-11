#ifndef MMBNTITLE_H
#define MMBNTITLE_H

#include "Screens.hpp"
#include "Animation.hpp"
#include "MMBNString.h"

#include <oslib/oslib.h>

class MMBNTitle : public Screen
{

	public:

		
		MMBNTitle()							;
		~MMBNTitle()						;
		virtual int	Run()					;
		void	Initialize()				;
		void	Destroy()					;
		
	private:
		
		enum MMBNTitleMenu
		{
			NEW_GAME,
			LOAD_GAME,
			EXIT_GAME
		};

		//Mise à jour de la scène (appelée par Run)
		virtual int	Update()	;
		//Affichage de la scène (appelée par Run)
		virtual void Display()	;
		
		//background
		AnimationPtr m_bg;						;

		MMBNString m_newGame					;
		MMBNString m_loadGame					;
		MMBNString m_exitGame					;
		
		OSL_IMAGE* m_cursor						;
		OSL_IMAGE* m_picture					;
		
		MMBNTitleMenu m_menu					;


};

#endif //TITLE_H
