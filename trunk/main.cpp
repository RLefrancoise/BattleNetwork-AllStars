#include <pspkernel.h>
#include <psprtc.h>
#include <oslib/oslib.h>
#include <string>
#include <map>

#include "Scene.h"
#include "Utils.h"
#include "Variables.h"
#include "GameSystem.h"
#include "MMBNBattleChip.h"
#include "SceneManager.h"

#include "LoadingScreen.h"

using namespace std;

//The callbacks
PSP_MODULE_INFO("KH PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int main()
{
#ifdef _DEBUG
	LOG("========================================")
	LOG("=============== KH PSP =================")
	LOG("========================================")
#endif

	//--------------INITIALIZE--------------------
	//Initialization of the library
	oslInit(0);
	
	//Initialisation du mode graphique
	oslInitGfx(OSL_PF_8888, 1);

	//Initialization of the text console
	oslInitConsole();

	//Initialize audio
	oslInitAudio();

	//Enable the no-fail feature
	oslSetQuitOnLoadFailure(1);

	srand(time(NULL));

	//-------------------------------------------------

	GameSystem::Initialize();
	MMBNBattleChip::LoadMaps();

	//Création des écrans de jeu
	/*std::map< int, Screen* > screens;
	int screen = SCREEN_TITLE;

	//Création de l'écran titre
	Title title;
	//Création de la battle map
	//BattleMap battleMap;
	MMBNBattleMap battleMap;
	//Création de la field map
	//FieldMap fieldMap;
	//fieldMap.Load("CentralArea");

	screens[SCREEN_TITLE] = &title;
	screens[SCREEN_BATTLEMAP] = &battleMap;
	//screens[SCREEN_FIELDMAP] = &fieldMap;*/

	ScreenPtr t(new Title());
	LoadingScreen* ls = new LoadingScreen(t);
	ScreenPtr screen(ls);

	SceneManager::getInstance()->pushScreen(screen);
	screen = SceneManager::getInstance()->getCurrentScreen();

	//--------------------------------------------

	//This variable holds whether a frame should be skipped or not
	int skip = 0;
	//Enable frameskipping, 0 frame out of 1 is skipped by default (60 fps)
	oslSetFrameskip(1);
	//But skip a maximum of 3 frames out of 4, else the game seems very laggy
	oslSetMaxFrameskip(4);

	//---------------FPS-------------------------
	u64 tempsDebut;
	sceRtcGetCurrentTick(&tempsDebut);
	
	int frames = 0;
	u64 temps;
	//float fps = 0;
	//-------------------------------------------

	//boucle principale
	while (!osl_quit && screen.get())
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

			//Run de l'écran en cours
			//screen = screens[screen]->Run();
			screen = SceneManager::getInstance()->getCurrentScreen();
			if(screen.get()) screen.get()->Run();
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

			ostringstream oss(ostringstream::out);
			oss << Variables::GetFPS();
			string s = "[FPS : " + oss.str() + "]";

			oslSetTextColor(RGBA(255,0,0,255));
			oslSetBkColor(RGBA(0,0,0,0));
			oslSetScreenClipping(0, 0, 480, 272);
			oslPrintf_xy(5,5,s.c_str());
		}
		//-------------------------------------------

		//We have now finished with drawing
        oslEndDrawing();

		//End of a frame
        oslEndFrame();
        //Synchronize to ensure the game runs at 60 fps
        skip = oslSyncFrame();

	}

	ImgManager::Reset();
	SndManager::Reset();
	FontManager::Reset();

	SceneManager::getInstance()->kill();

	oslEndGfx();
	oslQuit();
	sceKernelSleepThread();

#ifdef _DEBUG
	LOG("========================================")
	LOG("============== Shut down ===============")
	LOG("========================================")
#endif

	return 0;
}
