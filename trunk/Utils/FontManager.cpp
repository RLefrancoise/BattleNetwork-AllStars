#include "FontManager.h"
#include "Logger.h"
#include <string>

using namespace std;

std::map<std::string, MMBNFont*> FontManager::m_fontMap;

void FontManager::Reset()
{
	map<string, MMBNFont*>::iterator it;
	for(it = m_fontMap.begin() ; it != m_fontMap.end() ;++it)
		if(it->second) delete it->second;
		
	m_fontMap.clear();
	return;
}

MMBNFont* FontManager::GetFont(std::string name)
{

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
			LOG("Load font : " + name);
		}
	}

	//finalement, on retourne la police
	return m_fontMap[name];
}

