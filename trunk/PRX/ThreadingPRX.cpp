#include "ThreadingPRX.h"

void ThreadingPRX::displayPrxInfo(SceUID prxid)
{
	SceModule *mod;
	int i;

	mod = sceKernelFindModuleByUID(prxid);

	oslPrintf("mod %p\n", mod);

	if(mod != NULL)
	{
		oslPrintf("Attr %04X, Version %x.%x\n", mod->attribute, mod->version[0], mod->version[1]);
		oslPrintf("Name %s\n", mod->modname);
		oslPrintf("Text %08X, Size %08X, Data Size %08X\n", mod->text_addr, mod->text_size, mod->data_size);
		oslPrintf("Entry Addr %08X\n", mod->entry_addr);
		oslPrintf("Stub %p %08X, Ent %p %08X\n", mod->stub_top, mod->stub_size, mod->ent_top, mod->ent_size);
		for(i = 0; i < mod->nsegment; i++)
		{
			oslPrintf("Segment[%d] %08X %08X\n", i, mod->segmentaddr[i], mod->segmentsize[i]);
		}
	}
}
