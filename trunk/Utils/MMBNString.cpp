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
	unsigned int incX = 0, incY = 0;

	for(unsigned int i = 0 ; i < m_chars.size() ; i++)
	{
		if(m_chars[i] == '\n')
		{
			incX = 0;
			incY += m_font->GetCharHeight();
			continue;
		}
		if(m_chars[i] == ' ')
		{
			incX += m_font->GetCharWidth();
			continue;
		}
		
		pair<int,int> p = m_font->GetCharPosition(m_chars[i]);
		if( (p.first == -1) || (p.second == -1) ) continue;

		oslSetImageTile(m_font->GetFontPicture(), p.first * m_font->GetCharWidth(), p.second * m_font->GetCharHeight(), (p.first + 1) * m_font->GetCharWidth(), (p.second + 1) * m_font->GetCharHeight() );
		oslDrawImageXY(m_font->GetFontPicture(), m_position.x + incX + offX, m_position.y + incY + offY);
		incX += m_font->GetCharWidth();
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

const Vector2f& MMBNString::GetPosition() const
{
	return m_position;
}

unsigned int MMBNString::GetStringWidth()
{
	unsigned int width = 0;
	unsigned int max_width = 0;
	
	for(unsigned int i = 0 ; i < m_chars.size() ; i++)
	{
		if(m_chars[i] != '\n')
		{
			width += m_font->GetCharWidth();
			if(max_width < width) max_width = width;
		}
		else
			width = 0;
		
	}
	
	return max_width;
	
	//return (m_font->GetCharWidth() * m_chars.size());
}

unsigned int MMBNString::GetStringHeight()
{
	unsigned int height = m_font->GetCharHeight();
	
	for(unsigned int i = 0 ; i < m_chars.size() ; i++)
	{
		if(m_chars[i] == '\n')
			height += m_font->GetCharHeight();
	}
	
	return height;
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

void MMBNString::operator=(const char &c)
{
	m_chars.clear();
	
	if(m_font->ContainsChar(c))
		m_chars.push_back(c);
}

void MMBNString::operator=(const int &i)
{
	ostringstream oss(ostringstream::out);
	oss << i;
	string str = oss.str();
	
	m_chars.clear();

	for(unsigned int i = 0 ; i < str.size() ; i++)
		if(m_font->ContainsChar(str[i]))
			m_chars.push_back(str[i]);
	
	
}