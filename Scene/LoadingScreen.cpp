#include "LoadingScreen.h"
#include "SceneManager.h"
#include "ThreadingPRX.h"

#include <sstream>

using namespace std;

#define DEFAULT_THREAD_PRIORITY   32
#define DEFAULT_THREAD_STACK_KB_SIZE   256

ScreenPtr LoadingScreen::m_screen_to_load;

LoadingScreen::LoadingScreen()
{
	m_anim = Animation::Load("System/Animation/Loading/1");
	m_current_char = 0;
	m_loading_string = "Loading...";

	m_thread_created = false;
	m_thread_started = false;
	m_loading_done = false;

}

LoadingScreen::LoadingScreen(ScreenPtr& ptr)
{
	m_anim = Animation::Load("System/Animation/Loading/1");
	m_current_char = 0;
	m_loading_string = "Loading...";

	m_thread_created = false;
	m_thread_started = false;
	m_loading_done = false;

	SetScreenToLoad(ptr);
}

LoadingScreen::~LoadingScreen()
{
	if(m_anim) delete m_anim;
	if(m_thread_created) sceKernelExitDeleteThread(m_thread);
}

int LoadingScreen::Run()
{
	int screen = Update();
	if( (!m_loading_done) && (screen == SCREEN_LOADING) ) Display();

	return screen;
}



void LoadingScreen::Initialize()
{

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTROY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void LoadingScreen::Destroy()
{

}

int	LoadingScreen::Update()
{
	//=============================
	// GESTION CHARGEMENT
	//=============================
	/*if( m_thread_created )
	{
		if(!m_thread_started)
		{
			sceKernelStartThread(m_thread, m_args_size, m_argp);
			m_thread_started = true;
		}
		else
		{
			//get the status of the loading thread
			SceKernelThreadInfo info;
			info.size = sizeof(SceKernelThreadInfo);

			//acting according to the thread status
			int error = sceKernelReferThreadStatus(m_thread, &info);
			
			if( (error == 0) && (info.waitType < 1) )
			{
					m_loading_done = true;
					SceneManager::getInstance()->setScreen(m_screen_to_load);
			}
			
		}
	}*/

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

	return SCREEN_LOADING;
}

void LoadingScreen::Display()
{
	m_anim->Update();
	m_anim->SetPosition(470 - m_anim->GetCurrentSprite().GetHotPoint().x, 260);
	m_anim->Display();

	oslSetTextColor(RGBA(255,255,255,255));
	oslPrintf_xy(5, 260, m_loading_string.substr(0, m_current_char).c_str());
}

void LoadingScreen::SetScreenToLoad(ScreenPtr& s)
{
#ifdef _DEBUG
	SceUID tab[10];
	int cpt = 0;
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, tab, 10, &cpt);
	ostringstream oss(ostringstream::out);
	oss << cpt;
	LOG("Number of threads before creation: " + oss.str())
#endif

	if(m_thread_created) sceKernelTerminateDeleteThread(m_thread);

	m_screen_to_load = s;
	
	//m_thread = ThreadingPRX::createThread("loading_thread", LoadingScreen::Loading, DEFAULT_THREAD_PRIORITY, DEFAULT_THREAD_STACK_KB_SIZE, PSP_THREAD_ATTR_USER, NULL);
	m_thread = sceKernelCreateThread("loading_thread", LoadingScreen::Loading, DEFAULT_THREAD_PRIORITY, DEFAULT_THREAD_STACK_KB_SIZE, 0, NULL);
	(m_thread >= 0) ? m_thread_created = true : m_thread_created = false;
	
	m_thread_started = false;

#ifdef _DEBUG

	if(m_thread >= 0)
	{
		ostringstream oss(ostringstream::out);
		oss << m_thread;
		LOG("Create loading thread (ID: " + oss.str() + ")")
	}
	else 
		LOG("Can't create loading thread")


	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, tab, 10, &cpt);
	ostringstream oss2(ostringstream::out);
	oss2 << cpt;
	LOG("Number of threads after creation: " + oss2.str())

#endif

}

ScreenPtr& LoadingScreen::GetScreenToLoad()
{
	return m_screen_to_load;
}

int	LoadingScreen::Loading(SceSize size, void* argp)
{
	m_screen_to_load->Initialize();

	return 0;
}


void LoadingScreen::SetLoadingArguments(SceSize args_size, void* argp)
{
	m_args_size = args_size;
	m_argp = argp;
}