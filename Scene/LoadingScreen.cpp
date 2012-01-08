#include "LoadingScreen.h"
#include "Variables.h"

#include <sstream>

using namespace std;

Animation*		LoadingScreen::m_anim										;
unsigned int	LoadingScreen::m_current_char								;
std::string		LoadingScreen::m_loading_string								;
Timer			LoadingScreen::m_timer										;
		
LoadingScreen::LoadingScreen()
{
	#ifdef _DEBUG
		LOG("Create Loading Screen")
	#endif
	
	m_anim = Animation::Load("System/Animation/Loading/1");
	m_current_char = 0;
	m_loading_string = "Loading...";
	
	// the loadingscreen is loaded as a thread
	thid_ = sceKernelCreateThread("LoadingThread", RunLoadingScreen, 0x18, 0x10000, 0, NULL);
	// start the thread
	sceKernelStartThread(thid_, 0, 0);
	
}

LoadingScreen::~LoadingScreen()
{
	#ifdef _DEBUG
		LOG("Destroy Loading Screen")
	#endif
}

/*int LoadingScreen::Run()
{	
	int screen = Update();
	if( screen == SCREEN_LOADING ) Display();

	return screen;
}*/



/*void LoadingScreen::Initialize()
{

}*/

void LoadingScreen::KillLoadingScreen()
{
	// shut down the loading screen again.
	sceKernelTerminateDeleteThread(thid_);
	
	if(m_anim) delete m_anim;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/*void LoadingScreen::Destroy()
{
	
}*/

void LoadingScreen::Update()
{
	//=============================
	// GESTION AFFICHAGE
	//=============================
	if(!m_timer.is_started()) m_timer.start();
	
	if(m_timer.get_ticks() > 150) 
	{
		(m_current_char < m_loading_string.size()) ? m_current_char++ : m_current_char = 0;
		m_timer.stop();
		m_timer.start();
	}

}

void LoadingScreen::Display()
{
	m_anim->Update();
	m_anim->SetPosition(470 - m_anim->GetCurrentSprite().GetHotPoint().x, 260);
	m_anim->Display();

	oslSetTextColor(RGBA(255,255,255,255));
	oslPrintf_xy(5, 260, m_loading_string.substr(0, m_current_char).c_str());
}

int LoadingScreen::RunLoadingScreen(SceSize args, void *argp)
{
	
	int skip = 0;
	
	u64 tempsDebut;
	sceRtcGetCurrentTick(&tempsDebut);
	
	int frames = 0;
	u64 temps;
	
	while(true)
	{
	
		sceRtcGetCurrentTick(&temps);
		frames++;

		//Begin drawing
        oslStartDrawing();

		//If skip is 1, then we should skip rendering to speed up things
        if (!skip)              
		{
			//efface l'écran
			oslClearScreen(RGB(0,0,0));

			Update();
			Display();
        }
 
		//-----------------FPS----------------------
		u64 ticks;
		sceRtcGetCurrentTick(&ticks);

		if (ticks - tempsDebut > 1000)
		{
			sceRtcGetCurrentTick(&ticks);
			Variables::SetFPS( 1000000.0f * (float)frames / (float) (ticks - tempsDebut) );
			//fps=1000000.0f * (float)frames / (float) (ticks - tempsDebut);
			frames=0;

			sceRtcGetCurrentTick(&tempsDebut);

			#ifdef _DEBUG
				ostringstream oss(ostringstream::out);
				oss << Variables::GetFPS();
				string s = "[FPS : " + oss.str() + "]";

				oslSetTextColor(RGBA(255,0,0,255));
				oslSetBkColor(RGBA(0,0,0,0));
				oslSetScreenClipping(0, 0, 480, 272);
				oslPrintf_xy(5,5,s.c_str());
			#endif
		}
		//-------------------------------------------
		
		//We have now finished with drawing
        oslEndDrawing();

		//End of a frame
        oslEndFrame();
        //Synchronize to ensure the game runs at 60 fps
        skip = oslSyncFrame();
	
	}
	
	return 0;
}
