#include "ExtendedSprite.h"
#include "Logger.h"
#include "StringUtils.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

ExtendedSprite::ExtendedSprite()
{
	m_hotPoint.x = 0;
	m_hotPoint.y = 0;
	m_reverse_on_display = false;
}

/*ExtendedSprite(const sf::Image &image) : sf::Sprite(image)
{
	m_hotPoint.x = 0;
	m_hotPoint.y = 0;
}*/

ExtendedSprite::~ExtendedSprite()
{
}

void ExtendedSprite::SetHotPoint(Vector2i hp)
{
	m_hotPoint.x = hp.x;
	m_hotPoint.y = hp.y;
}

void ExtendedSprite::AddDefenceBox(AABBCollisionBox box)
{
	m_defenceBoxes.push_back(box);
}

void ExtendedSprite::AddOffenseBox(AABBCollisionBox box)
{
	m_offenseBoxes.push_back(box);
}

void ExtendedSprite::ReverseCollisionsBoxes()
{
	std::vector<AABBCollisionBox>::iterator it;

	for(it = m_defenceBoxes.begin() ; it != m_defenceBoxes.end() ; ++it)
	{
		int left = it->Left, right = it->Left + it->Width;

		it->Left = (m_sprite->sizeX + m_sprite->offsetX0) - right;
		it->Width = (m_sprite->offsetX0 + m_sprite->sizeX) - left - it->Left;

	}

	for(it = m_offenseBoxes.begin() ; it != m_offenseBoxes.end() ; ++it)
	{
		int left = it->Left, right = it->Width;

		it->Left = (m_sprite->sizeX + m_sprite->offsetX0) - right;
		it->Width = (m_sprite->offsetX0 + m_sprite->sizeX) - left - it->Left;
	}

	m_hotPoint.x = m_sprite->sizeX - m_hotPoint.x;
}

void ExtendedSprite::Display(float offX, float offY)
{
	if(m_sprite)
	{
		if(m_reverse_on_display) oslMirrorImageH(m_sprite);
		oslDrawImageXY(m_sprite, m_sprite->x + offX, m_sprite->y + offY);
		if(m_reverse_on_display) oslMirrorImageH(m_sprite);
	}
}

void ExtendedSprite::DisplayExtension()
{
	//Hot Point
	oslDrawFillRect( m_sprite->x + m_hotPoint.x - 1, m_sprite->y + m_hotPoint.y - 1, m_sprite->x + m_hotPoint.x + 1, m_sprite->y + m_hotPoint.y + 1, RGB(255,0,0) );
	/*sf::Shape hotPoint = sf::Shape::Circle(this->GetPosition().x + m_hotPoint.x, this->GetPosition().y + m_hotPoint.y, 2, sf::Color(255,0,0));
	App.Draw(hotPoint);

	//Defence Boxes
	std::vector<AABBCollisionBox>::iterator it;
	for(it = m_defenceBoxes.begin() ; it != m_defenceBoxes.end() ; ++it)
	{
		sf::Shape box = sf::Shape::Rectangle( it->Left, it->Top, it->Width, it->Height, sf::Color(0,0,255), 1, sf::Color(0,0,255) );
		box.EnableFill(false);
		box.EnableOutline(true);
		box.SetPosition(this->GetPosition().x, this->GetPosition().y);
		App.Draw(box);
	}*/
		
}

void ExtendedSprite::SetReverseOnDisplay(bool enable)
{
	m_reverse_on_display = enable;
}

void ExtendedSprite::LoadCollisions(int frameNb, std::string path)
{
	ifstream in_colli( (path + "/collisions.txt").c_str() , ifstream::in);

	if(!in_colli.good())
	{
		//LOG("Impossible de trouver le fichier " + path);
		//exit(EXIT_FAILURE);
		return;
	}

	string line;
	bool frameDefFound = false;

	while(!frameDefFound && getline(in_colli, line))
	{
		ostringstream oss;
		oss << frameNb;

		if(line.find("Frame " + oss.str()) == 0)
		{
			frameDefFound = true;

			while(getline(in_colli, line) && (line.find("Frame") != 0))
			{
				if(line.find("hotPoint") != 0)
				{
					vector<string> v = StringUtils::Split(line, ",");

					istringstream iss(v.at(0) + " " + v.at(1) + " " + v.at(2) + " " + v.at(3));
					int x, y, w, h;
					iss >> x;
					iss >> y;
					iss >> w;
					iss >> h;

					//AABBCollisionBox box(x, y, w + x, h + y);
					AABBCollisionBox box;
					box.Left = x;
					box.Top = y;
					box.Width = w;
					box.Height = h;

					if(v.at(4).compare("DEFENCE") == 0)
						AddDefenceBox(box);
					else
						AddOffenseBox(box);
				}
				else if(line.find("hotPoint") == 0)
				{
					vector<string> v = StringUtils::Split(line, " ");

					istringstream iss(v.at(0) + " " + v.at(1) + " " + v.at(2));
					string s;
					int x, y;

					iss >> s;
					iss >> x;
					iss >> y;

					SetHotPoint(Vector2i(x, y));
				}
				
				
			}
		}
	}

	in_colli.close();
}

void ExtendedSprite::SetPosition(float X, float Y)
{
	m_sprite->x = X - m_hotPoint.x;
	m_sprite->y = Y - m_hotPoint.y;
}

void ExtendedSprite::SetImage(OSL_IMAGE* img)
{
	m_sprite = img;
}

std::vector<AABBCollisionBox> ExtendedSprite::GetDefenceBoxes() const
{
	return m_defenceBoxes;
}

std::vector<AABBCollisionBox> ExtendedSprite::GetOffenseBoxes() const
{
	return m_offenseBoxes;
}

Vector2i ExtendedSprite::GetHotPoint() const
{
	return m_hotPoint;
}

Vector2i ExtendedSprite::GetSize() const
{
	Vector2i v;
	v.x = m_sprite->sizeX;
	v.y = m_sprite->sizeY;
	return v;
}