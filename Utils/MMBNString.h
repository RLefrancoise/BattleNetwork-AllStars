#ifndef MMBN_STRING_H
#define MMBN_STRING_H

#include <string>

#include "MMBNFont.h"
#include "Displayable.h"
#include "Vector2f.h"

class MMBNString : public Displayable
{
	private:
		MMBNFont* m_font;
		std::string m_chars;

		Vector2f m_position;

	public:
		MMBNString();
		~MMBNString();

		void SetFont(MMBNFont* f);
		MMBNFont* GetFont();

		void Display(float offX = 0, float offY = 0);
		void Move(float x, float y);
		void SetPosition(float x, float y);
		Vector2f& GetPosition();

		unsigned int GetStringWidth();

		void SetString(std::string& str);

		void operator=(const char* &str);
		void operator=(const std::string &str);
		void operator=(const char &c);
		void operator=(const int &i);
};

#endif
