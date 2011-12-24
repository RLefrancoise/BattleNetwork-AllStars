#ifndef THREADINGPRX_H
#define THREADINGPRX_H

#include <oslib/oslib.h>
#include <pspthreadman.h>

/*class ThreadingPRX
{
	public:
		static SceUID createThread(const char* name, SceKernelThreadEntry entry, int priority, int stacksize, SceUInt attr, SceKernelThreadOptParam* option);
		static int deleteThread(SceUID thid);
		static int startThread(SceUID thid, SceSize arglen, void* argp);
		static int getThreadInfo(SceUID thid, SceKernelThreadInfo* info);
		static void* getModuleInfo(void);

		static void displayPrxInfo(SceUID prxid);
};*/


SceUID createThread(const char* name, SceKernelThreadEntry entry, int priority, int stacksize, SceUInt attr, SceKernelThreadOptParam* option);
int deleteThread(SceUID thid);
int startThread(SceUID thid, SceSize arglen, void* argp);
int getThreadInfo(SceUID thid, SceKernelThreadInfo* info);
void* getModuleInfo(void);

void displayPrxInfo(SceUID prxid);

#endif
