#include "MMBNFont.h"

#include "Utils.h"

#include <fstream>

using namespace std;

MMBNFont::MMBNFont()
{
	m_font_pic = NULL;
	m_char_width = 0;
	m_char_height = 0;
}

MMBNFont::~MMBNFont()
{
}

MMBNFont* MMBNFont::Load(string name/*, unsigned int char_w, unsigned int char_h*/)
{
	MMBNFont* f = new MMBNFont();

	f->m_name = name;

	f->m_font_pic = ImgManager::GetImage("Font/" + name + "/font.png");

	//f->m_char_width = char_w;
	//f->m_char_height = char_h;

	//f->InitCharMap();
	f->InitInfos();

	return f;

}

void MMBNFont::InitCharMap()
{
	m_char_map['0'] = pair<int,int>(0,0);
	m_char_map['1'] = pair<int,int>(1,0);
	m_char_map['2'] = pair<int,int>(2,0);
	m_char_map['3'] = pair<int,int>(3,0);
	m_char_map['4'] = pair<int,int>(4,0);
	m_char_map['5'] = pair<int,int>(5,0);
	m_char_map['6'] = pair<int,int>(6,0);
	m_char_map['7'] = pair<int,int>(7,0);
	m_char_map['8'] = pair<int,int>(8,0);
	m_char_map['9'] = pair<int,int>(9,0);

}

void MMBNFont::InitInfos()
{
	ifstream in_infos( (string("Font/") + m_name + string("/info.txt")).c_str() , ifstream::in);
	
	if(!in_infos.good())
	{
		LOG("Impossible de trouver le fichier info.txt pour " + string("Font/") + m_name + string("/"));
		oslQuit();
	}

	string line;
	while(getline(in_infos, line))
	{
		if(line.find("size") == 0)
		{
			istringstream iss(line);
			string size;
			iss >> size;
			iss >> m_char_width;
			iss >> m_char_height;
		}
		else
		{
			istringstream iss(line);
			char c;
			pair<int,int> p;
			iss >> c;
			iss >> p.first;
			iss >> p.second;

			m_char_map[c] = p;
		}
	}

	in_infos.close();
}

bool MMBNFont::ContainsChar(char c)
{
	if( (c == '\n') || (c == ' ') ) return true;

	map<char, pair<int,int> >::iterator it;

	it = m_char_map.find(c);

	return ( it != m_char_map.end() );
}

unsigned int MMBNFont::GetCharWidth()
{
	return m_char_width;
}

unsigned int MMBNFont::GetCharHeight()
{
	return m_char_height;
}

OSL_IMAGE* MMBNFont::GetFontPicture() const
{
	return m_font_pic;
}

pair<int,int> MMBNFont::GetCharPosition(char c)
{
	if(ContainsChar(c)) return m_char_map[c];
	
	pair<int,int> p(-1, -1);
	return p;
}