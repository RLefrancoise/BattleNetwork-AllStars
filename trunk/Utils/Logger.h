#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <oslib/oslib.h>
#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <psprtc.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include "Singleton.h"

#define LOG(msg)	Logger::Write(msg);

class Logger
{

    protected:

        Logger()						;
        virtual ~Logger() = 0			;
		
		static SceUID m_log_sema		;
		static bool	m_initialized		;
		
		static void Initialize()		;
		static void LockSema()			;
		static void UnlockSema()		;

    public:

        static void Write(std::string msg)	;
};

#endif
