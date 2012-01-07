#include "SndManager.h"
#include "Logger.h"
#include <string>

using namespace std;

OSL_SOUND* SndManager::m_bgmTab[8];
std::string SndManager::m_bgmNameTab[8];

void SndManager::Initialize()
{
	for(unsigned int i = 0 ; i < 8 ; i++)
		m_bgmTab[i] = NULL;
	
}

void SndManager::Reset()
{
	for(unsigned int i = 0 ; i < 8 ; i++)
	{
		if(m_bgmTab[i]) oslDeleteSound(m_bgmTab[i]);
	}

	return;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// PLAY BGM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void SndManager::PlayBGM(std::string name, int voice , bool loop)
{
	if(strcmp(m_bgmNameTab[voice].c_str(),name.c_str()) == 0) return;

	m_bgmNameTab[voice] = name;

	if(m_bgmTab[voice]) oslDeleteSound(m_bgmTab[voice]);

	m_bgmTab[voice] = oslLoadSoundFile((string("Audio/BGM/") + name).c_str(), OSL_FMT_NONE);
	if(!m_bgmTab[voice])
	{
		LOG("Can't load sound Audio/BGM/" + name)
		oslQuit();
	}
	
	if(loop) oslSetSoundEndCallback(m_bgmTab[voice], loopCallback);

	oslPlaySound(m_bgmTab[voice],voice);

}

void SndManager::StopBGM(int voice)
{
	oslStopSound(m_bgmTab[voice]);
}

int SndManager::loopCallback(OSL_SOUND *s, int voice)         {
    //We replay the same sound on the same voice
    oslPlaySound(s, voice);
    //Return 1 because we continue sound playback
    return 1;
}