#include "FontManager.h"
#include "Logger.h"
#include <string>

using namespace std;

std::map<std::string, MMBNFont*> FontManager::m_fontMap;

SceUID FontManager::m_font_sema = -1;
bool FontManager::m_initialized = false;

void FontManager::Initialize()
{
	m_font_sema = sceKernelCreateSema("font_manager_sem", 0, 1, 1, NULL);
	if(m_font_sema < 0) oslQuit();
	
	m_initialized = true;
}

void FontManager::LockSema()
{
	int ret = sceKernelWaitSema(m_font_sema, 1, 0);
	if (ret < 0) printf("sceKernelWaitSema(%08x) failed with %08x\n", m_font_sema, ret);
}

void FontManager::UnlockSema()
{
	int ret = sceKernelSignalSema(m_font_sema, 1);
	if (ret < 0) printf("sceKernelSignalSema(%08x) failed with %08x\n", m_font_sema, ret);
}



void FontManager::Reset()
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	map<string, MMBNFont*>::iterator it;
	for(it = m_fontMap.begin() ; it != m_fontMap.end() ;++it)
		if(it->second) delete it->second;
		
	m_fontMap.clear();
	
	UnlockSema();
	
	#ifdef _DEBUG
	LOG("Reset FontManager")
	#endif
	
	return;
}

MMBNFont* FontManager::GetFont(std::string name)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	//police déjà chargée ?
	map<string, MMBNFont*>::iterator it;
	it = m_fontMap.find(name);

	//si non
	if(it == m_fontMap.end())
	{
		//on charge la police
		MMBNFont* font = MMBNFont::Load(name);
		if(!font) //si le chargement échoue on log et on quitte
		{
			LOG("Can't load font : " + name);
			exit(EXIT_FAILURE);
		}
		else //si le chargement a réussi on stocke dans la map
		{
			pair<string, MMBNFont*> p;
			p.first = name;
			p.second = font;
			m_fontMap.insert(p);
			#ifdef _DEBUG
				LOG("Load font : " + name);
			#endif 
		}
	}

	UnlockSema();
	
	//finalement, on retourne la police
	return m_fontMap[name];
}

