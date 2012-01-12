#ifndef MMBNCHARACTERSELECT_H
#define MMBNCHARACTERSELECT_H

#include "Screens.hpp"
#include "Animation.hpp"
#include "MMBNString.h"

#include <oslib/oslib.h>

class CharacterSelectInfo
{
	public:
		std::string m_name;
		OSL_IMAGE* m_icon;
		OSL_IMAGE* m_body;
		CharacterSelectInfo();
		CharacterSelectInfo(std::string name, OSL_IMAGE* icon, OSL_IMAGE* body);
		
};

typedef boost::shared_ptr<CharacterSelectInfo> CharacterSelectInfoPtr		;

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
		
		//cursors
		AnimationPtr 	m_actor_cursor				;
		AnimationPtr 	m_enemy_cursor				;
		
		//pictures
		OSL_IMAGE*		m_chara_frame				;
		
		//variables
		unsigned int 	m_current_bgm				;
		bool			m_can_plug_in				;
		
		static const int		CHARA_PER_LINE = 10			;
		static const int		CHARA_ICON_SIZE = 32		;
		
		std::vector<CharacterSelectInfoPtr> m_characters;
		std::vector<Vector2i> m_chara_positions		;

};

#endif //TITLE_H
