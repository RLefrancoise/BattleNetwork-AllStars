#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <oslib/oslib.h>
#include <pspthreadman.h>

#include "Animation.hpp"

class LoadingScreen
{

	private:

		static 	AnimationPtr		m_anim									;
		static 	unsigned int	m_current_char								;
		static 	std::string		m_loading_string							;

		static 	Timer			m_timer										;

		static void				Update()									;
		static void				Display()									;
		
		static int 				RunLoadingScreen(SceSize args, void *argp)	;
		SceUID 					thid_										;

	public:
		LoadingScreen()														;
		~LoadingScreen()													;
		
		void 					KillLoadingScreen()							;
		
};

#endif
