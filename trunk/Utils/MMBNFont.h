#ifndef MMBN_FONT_H
#define MMBN_FONT_H

#include <oslib/oslib.h>
#include <map>
#include <string>

class MMBNFont
{

	private:
		OSL_IMAGE* m_font_pic;
		unsigned int m_char_width;
		unsigned int m_char_height;
		std::string m_name;
		std::map<char,std::pair<int,int> > m_char_map;

		MMBNFont();

		void InitCharMap();
		void InitInfos();

	public:
		~MMBNFont();
		static MMBNFont* Load(std::string name/*, unsigned int char_w, unsigned int char_h*/);

		bool ContainsChar(char c);
		unsigned int GetCharWidth();
		unsigned int GetCharHeight();
		OSL_IMAGE* GetFontPicture() const;
		std::pair<int,int> GetCharPosition(char c);

};

#endif
