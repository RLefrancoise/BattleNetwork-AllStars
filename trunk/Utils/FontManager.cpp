#include "FontManager.h"
#include "Logger.h"
#include <string>

using namespace std;

//std::map<std::string, sf::Font> FontManager::m_fontMap;

void FontManager::Reset()
{
	//m_fontMap.clear();
	return;
}

/*sf::Font& FontManager::getFont(std::string name)
{

	//police déjà chargée ?
	map<string, sf::Font>::iterator it;
	it = m_fontMap.find(name);

	//si non
	if(it == m_fontMap.end())
	{
		//on charge la police
		sf::Font font;
		if(!font.LoadFromFile(name)) //si le chargement échoue on log et on quitte
		{
			LOG("Can't load font : " + name);
			exit(EXIT_FAILURE);
		}
		else //si le chargement a réussi on stocke dans la map
		{
			pair<string, sf::Font> p;
			p.first = name;
			p.second = font;
			m_fontMap.insert(p);
			LOG("Load font : " + name);
		}
	}

	//finalement, on retourne la police
	return m_fontMap[name];
}

*/