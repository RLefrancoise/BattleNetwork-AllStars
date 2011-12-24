	.set noreorder

#include "pspstub.s"

	STUB_START "ThreadingFunc",0x40090000,0x00050005
	STUB_FUNC  0x712871DF,createThread
	STUB_FUNC  0xF7B5B929,deleteThread
	STUB_FUNC  0x83D2B550,startThread
	STUB_FUNC  0xE1B31505,getThreadInfo
	STUB_FUNC  0x563FF2B2,getModuleInfo
	STUB_END
