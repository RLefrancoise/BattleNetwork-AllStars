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

#define DEFAULT_THREAD_PRIORITY   32
#define DEFAULT_THREAD_STACK_KB_SIZE   256

using namespace std;

//The callbacks
PSP_MODULE_INFO("KH PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

//Création des écrans de jeu
std::map< int, Screen* > screens;
int screen = SCREEN_TITLE;
int previousScreen = screen;
	
SceUID loadingThread;
bool screenLoaded = false;
bool loadingThreadCreated = false;

int Loading(SceSize size, void* argp)
{
	LOG("Enter loading function")
	//Screen *s = *((Screen **) argp);
	//Screen* s = (Screen*) argp;
	
	LOG("Initialize screen")
	screens[screen]->Initialize();
	//s->Initialize();
	//s->LoadRessources();
	
	LOG("Initialize done")
	screenLoaded = true;
	loadingThreadCreated = false;
	LOG("Exit loading function")
	return 0;
}

int shutdownCallback(int arg1, int arg2, void* common)
{
	ImgManager::Reset();
	SndManager::Reset();
	FontManager::Reset();

	SceneManager::getInstance()->kill();
	
#ifdef _DEBUG
	LOG("========================================")
	LOG("============== Shut down ===============")
	LOG("========================================")
#endif
	
	return 0;
}
 
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

	oslSetExitCallback(shutdownCallback);
	
	srand(time(NULL));

	//-------------------------------------------------

	GameSystem::Initialize();
	MMBNBattleChip::LoadMaps();
		
	LoadingScreen loadingScreen;
	
	//Création de l'écran titre
	Title title;
	//Création de la battle map
	//BattleMap battleMap;
	MMBNBattleMap battleMap;
	//Création de la field map
	FieldMap fieldMap;

	screens[SCREEN_TITLE] = &title;
	screens[SCREEN_BATTLEMAP] = &battleMap;
	screens[SCREEN_LOADING] = &loadingScreen;
	screens[SCREEN_FIELDMAP] = &fieldMap;

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
	while (!osl_quit && screen != SCREEN_EXIT)
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

			//----------------DEBUG----------------------
			ostringstream oss(ostringstream::out);
			oss << "Screen Loaded: " << screenLoaded << " Loading Thread Created: " << loadingThreadCreated << " Screen: " << screen << " PScreen: " << previousScreen;
			oslPrintf_xy(5, 15, oss.str().c_str());
			
			
			//Run de l'écran en cours si le chargement est fini
			if(screenLoaded)
			{	
				previousScreen = screen;
				screen = screens[screen]->Run();
			}
			
			//sinon on charge
			else
			{
				//si le thread de chargement n'est pas encore créé, on le crée
				if(!loadingThreadCreated)
				{
					screens[previousScreen]->Destroy();
					
					loadingThread = sceKernelCreateThread("loading_thread", Loading, DEFAULT_THREAD_PRIORITY, DEFAULT_THREAD_STACK_KB_SIZE, PSP_THREAD_ATTR_USER, NULL);
					loadingThreadCreated = true;
					LOG("Loading Thread Created")
					if(loadingThread >= 0) sceKernelStartThread(loadingThread, 0, NULL);
					LOG("Loading Thread Started")
				}
						
				//on affiche l'écran de chargement
				screens[SCREEN_LOADING]->Run();
			}
				
			//l'écran a changé ?
			if(screen != previousScreen)
			{
				screenLoaded = false;
				
			}
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

	/*ImgManager::Reset();
	SndManager::Reset();
	FontManager::Reset();

	SceneManager::getInstance()->kill();
	
#ifdef _DEBUG
	LOG("========================================")
	LOG("============== Shut down ===============")
	LOG("========================================")
#endif*/

	oslEndGfx();
	oslQuit();
	sceKernelSleepThread();

	return 0;
}
