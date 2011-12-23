#ifndef FONTMANAGER_H_INCLUDED
#define FONTMANAGER_H_INCLUDED

#include <string>
#include <map>

#include "MMBNFont.h"

class FontManager
{

	public:
		static void Reset();
		static MMBNFont* GetFont(std::string name);

	private:
		
		static std::map<std::string, MMBNFont*> m_fontMap;
		FontManager();
		virtual ~FontManager() = 0;
};

#endif
