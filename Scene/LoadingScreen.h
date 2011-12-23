#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <oslib/oslib.h>
#include <pspthreadman.h>

#include "Screens.hpp"
#include "Animation.hpp"

class LoadingScreen : public Screen
{

	private:
		
		SceUID			m_thread									;
		bool			m_thread_created							;
		SceSize			m_args_size									;
		void*			m_argp										;

		static ScreenPtr		m_screen_to_load							;
		static bool			m_loading_done								;

		Animation*		m_anim										;
		unsigned int	m_current_char								;
		std::string		m_loading_string							;

		Timer			m_timer										;

		int		Update()											;
		void	Display()											;

		static int		Loading(SceSize size, void* argp)			;

	public:
		LoadingScreen()												;
		LoadingScreen(ScreenPtr& ptr)								;
		~LoadingScreen()											;

		int		Run()												;
		void	Initialize()										;

		void	SetScreenToLoad(ScreenPtr& s)						;
		ScreenPtr& GetScreenToLoad()								;

		void	SetLoadingArguments(SceSize args_size, void* argp)	;


};

#endif
