#include "SndManager.h"
#include "Logger.h"
#include <string>

using namespace std;

OSL_SOUND* SndManager::m_bgmTab[8];
std::string SndManager::m_bgmNameTab[8];

SceUID SndManager::m_snd_sema = -1;
bool SndManager::m_initialized = false;

void SndManager::Init()
{
	m_snd_sema = sceKernelCreateSema("snd_manager_sem", 0, 1, 1, NULL);
	if(m_snd_sema < 0) oslQuit();
	
	m_initialized = true;
}

void SndManager::LockSema()
{
	int ret = sceKernelWaitSema(m_snd_sema, 1, 0);
	if (ret < 0) printf("sceKernelWaitSema(%08x) failed with %08x\n", m_snd_sema, ret);
}

void SndManager::UnlockSema()
{
	int ret = sceKernelSignalSema(m_snd_sema, 1);
	if (ret < 0) printf("sceKernelSignalSema(%08x) failed with %08x\n", m_snd_sema, ret);
}





void SndManager::Initialize()
{
	if(!m_initialized) Init();
	
	LockSema();
	
	for(unsigned int i = 0 ; i < 8 ; i++)
		m_bgmTab[i] = NULL;
		
	UnlockSema();
	
}

void SndManager::Reset()
{
	if(!m_initialized) Init();
	
	LockSema();
	
	for(unsigned int i = 0 ; i < 8 ; i++)
	{
		if(m_bgmTab[i]) oslDeleteSound(m_bgmTab[i]);
	}

	UnlockSema();
	
	#ifdef _DEBUG
	LOG("Reset SndManager")
	#endif
	
	return;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// PLAY BGM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void SndManager::PlayBGM(std::string name, int voice , bool loop)
{
	if(!m_initialized) Init();
	
	LockSema();
	
	if(strcmp(m_bgmNameTab[voice].c_str(),name.c_str()) == 0) { UnlockSema(); return; }

	m_bgmNameTab[voice] = name;

	if(m_bgmTab[voice]) oslDeleteSound(m_bgmTab[voice]);

	m_bgmTab[voice] = oslLoadSoundFile((string("Audio/BGM/") + name).c_str(), OSL_FMT_STREAM);
	if(!m_bgmTab[voice])
	{
		LOG("Can't load sound Audio/BGM/" + name)
		oslQuit();
	}
	
	if(loop) oslSetSoundEndCallback(m_bgmTab[voice], loopCallback);

	oslPlaySound(m_bgmTab[voice],voice);
	
	UnlockSema();

}

void SndManager::StopBGM(int voice)
{
	if(!m_initialized) Init();
	
	LockSema();
	
	oslStopSound(m_bgmTab[voice]);
	m_bgmNameTab[voice] = "";
	
	UnlockSema();
}

int SndManager::loopCallback(OSL_SOUND *s, int voice)
{
	if(!m_initialized) Init();
	
	LockSema();
	
    //We replay the same sound on the same voice
    oslPlaySound(s, voice);
	
	UnlockSema();
	
    //Return 1 because we continue sound playback
    return 1;
}