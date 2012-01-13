#ifndef MMBNCHARACTERSELECT_H
#define MMBNCHARACTERSELECT_H

#include "Screens.hpp"
#include "Animation.hpp"
#include "MMBNString.h"

#include <oslib/oslib.h>
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

class CharacterSelectInfo
{
	public:
		
		CharacterSelectInfo();
		CharacterSelectInfo(std::string name, OSL_IMAGE* icon, OSL_IMAGE* body, Vector2i pos);
		~CharacterSelectInfo();
		std::string GetName();
		OSL_IMAGE* GetIcon();
		OSL_IMAGE* GetBody();
		Vector2i GetPosition();
		
	private:
		std::string m_name;
		OSL_IMAGE* m_icon;
		OSL_IMAGE* m_body;
		Vector2i m_position;
		
};

typedef boost::shared_ptr<CharacterSelectInfo> 							CharacterSelectInfoPtr	;
typedef boost::multi_array< CharacterSelectInfoPtr, 1 >					CharactersInfo			;
typedef boost::shared_ptr<CharactersInfo>								CharactersInfoPtr		;

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
		AnimationPtr 		m_bg						;
		AnimationPtr 		m_plug_in					;
		
		//cursors
		AnimationPtr 		m_actor_cursor				;
		AnimationPtr 		m_enemy_cursor				;
		
		//pictures
		OSL_IMAGE*			m_chara_frame				;
		
		//texts
		//MMBNString			m_title						;
		
		//variables
		bool				m_can_plug_in				;
		
		unsigned int		CHARA_PER_LINE				;
		unsigned int		CHARA_ICON_SIZE				;
		
		int					m_current_actor				;
		int					m_current_enemy				;
		int 				m_current_bgm				;
		int					m_current_background		;
		
		bool				m_is_selecting_actor		;
		bool				m_is_selecting_enemy		;
		
		CharactersInfoPtr 	m_characters				;
		unsigned int 		m_chara_nb					;
		
		
		void				MoveCursor(OSL_CONTROLLER* k, int* cursor)		;

};

#endif //TITLE_H
