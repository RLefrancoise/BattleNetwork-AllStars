#ifndef SNDMANAGER_H_INCLUDED
#define SNDMANAGER_H_INCLUDED

#include <string>
#include <map>

#include <oslib/oslib.h>

class SndManager
{

	public:
		static void Initialize();
		static void PlayBGM(std::string name, int voice , bool loop);
		static void StopBGM(int voice);
		static void Reset();

	private:

		static OSL_SOUND* m_bgmTab[8];
		static std::string m_bgmNameTab[8];

		SndManager();
		virtual ~SndManager() = 0;

		static int loopCallback(OSL_SOUND* s, int voice);
};

#endif
