#include "MMBNString.h"

using namespace std;

MMBNString::MMBNString()
{
	m_font = NULL;
}

MMBNString::~MMBNString()
{

}

void MMBNString::SetFont(MMBNFont* f)
{
	m_font = f;
}

MMBNFont* MMBNString::GetFont()
{
	return m_font;
}

void MMBNString::Display(float offX, float offY)
{
	unsigned int inc = 0;

	for(unsigned int i = 0 ; i < m_chars.size() ; i++)
	{
		pair<int,int> p = m_font->GetCharPosition(m_chars[i]);
		if( (p.first == -1) || (p.second == -1) ) continue;

		oslSetImageTile(m_font->GetFontPicture(), p.first * m_font->GetCharWidth(), p.second * m_font->GetCharHeight(), (p.first + 1) * m_font->GetCharWidth(), (p.second + 1) * m_font->GetCharHeight() );
		oslDrawImageXY(m_font->GetFontPicture(), m_position.x + inc, m_position.y);
		inc += m_font->GetCharWidth();
	}
}

void MMBNString::Move(float x, float y)
{
	m_position.x += x;
	m_position.y += y;
}

void MMBNString::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

Vector2f& MMBNString::GetPosition()
{
	return m_position;
}

unsigned int MMBNString::GetStringWidth()
{
	return (m_font->GetCharWidth() * m_chars.size());
}

void MMBNString::SetString(std::string& str)
{
	m_chars.clear();

	for(unsigned int i = 0 ; i < str.size() ; i++)
		if(m_font->ContainsChar(str[i]))
			m_chars.push_back(str[i]);
}

void MMBNString::operator=(const char* &str)
{

	m_chars.clear();

	for(unsigned int i = 0 ; str[i] == '\0' ; i++)
		if(m_font->ContainsChar(str[i]))
			m_chars.push_back(str[i]);
		
}

void MMBNString::operator=(const string &str)
{

	m_chars.clear();

	for(unsigned int i = 0 ; i < str.size() ; i++)
		if(m_font->ContainsChar(str[i]))
			m_chars.push_back(str[i]);
		
}
