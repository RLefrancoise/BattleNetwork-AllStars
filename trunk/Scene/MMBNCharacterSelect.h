#ifndef MMBNCHARACTERSELECT_H
#define MMBNCHARACTERSELECT_H

#include "Screens.hpp"
#include "Animation.hpp"
#include "MMBNString.h"

#include <oslib/oslib.h>

class MMBNCharacterSelect : public Screen
{

	public:

		
		MMBNCharacterSelect()				;
		~MMBNCharacterSelect()				;
		virtual int	Run()					;
		void	Initialize()				;
		void	Destroy()					;
		
	private:
		
		//Mise à jour de la scène (appelée par Run)
		virtual int	Update()	;
		//Affichage de la scène (appelée par Run)
		virtual void Display()	;
		
		//background
		AnimationPtr 	m_bg						;
		AnimationPtr 	m_plug_in					;
		
		AnimationPtr 	m_actor_cursor				;
		AnimationPtr 	m_enemy_cursor				;
		
		unsigned int 	m_current_bgm				;
		
		bool			m_can_plug_in				;

};

#endif //TITLE_H
