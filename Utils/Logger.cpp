#include "Logger.h"

#include <psputils.h>

using namespace std;

SceUID Logger::m_log_sema = -1;
bool Logger::m_initialized = false;

void Logger::Initialize()
{
	m_log_sema = sceKernelCreateSema("logger_sem", 0, 1, 1, NULL);
	if(m_log_sema < 0) oslQuit();
	
	m_initialized = true;
}

void Logger::LockSema()
{
	int ret = sceKernelWaitSema(m_log_sema, 1, 0);
	if (ret < 0) printf("sceKernelWaitSema(%08x) failed with %08x\n", m_log_sema, ret);
}

void Logger::UnlockSema()
{
	int ret = sceKernelSignalSema(m_log_sema, 1);
	if (ret < 0) printf("sceKernelSignalSema(%08x) failed with %08x\n", m_log_sema, ret);
}

void Logger::Write(string msg)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	pspTime t;
	sceRtcGetCurrentClockLocalTime(&t);
	
	SceUID log = sceIoOpen("Log.txt", PSP_O_WRONLY|PSP_O_CREAT|PSP_O_APPEND, 0777);
	
	if(log < 0)  oslQuit();
	
	ostringstream oss(ostringstream::out);
	oss << "[" << t.year << "/" << t.month << "/" << t.day << " " << t.hour << ":" << t.minutes << ":" << t.seconds << "]" << msg << "\r\n";
	
	string s(oss.str());
	sceIoWrite(log, s.c_str(), sizeof(char) * s.size());
	
	sceIoClose(log);
	
	UnlockSema();
}
