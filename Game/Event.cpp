#include "Event.h"
#include "ImgManager.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// CONSTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
Event::Event()
{
}

Event::Event(string name)
{
	m_name = name; // /!\ à mettre au début car les autres fonctions en ont besoin

	m_posX = 0;
	m_posY = 0;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DESTRUCTEUR
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

Event::~Event()
{

	//=========================================
	// SUPPRESSION DES ANIMATIONS
	//=========================================
	std::map<string,Animation*>::iterator it;
	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).second ) delete (*it).second;


}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
Event* Event::Load(std::string name)
{
	return new Event(name);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// INIT DELAYS OF ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Event::InitDelaysOfAnim(string name, vector<int> &delays)
{
	int spritesNb = 0, delay = 0;
	ifstream in_anim( (string("Files/Events/") + m_name + string("/") + name + string("/anim.txt")).c_str() , ifstream::in);

	if(!in_anim.good())
	{
		LOG("Impossible de trouver le fichier anim.txt pour " + string("Files/Events/") + m_name + string("/") + name);
		//exit(EXIT_FAILURE);
	}

	string line;
	while(getline(in_anim, line))
	{
		if(line.find("sprites=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			istringstream iss(line);
			iss >> spritesNb;
		}
		else if(line.find("delays=") == 0)
		{
			int equalsIndex = line.find_first_of("=");
			line = line.substr(equalsIndex + 1);
			int commaIndex = -1;

			for(int i = 0 ; i < spritesNb ; i++)
			{
				commaIndex = line.find_first_of(",");

				string delay_s;
				if(commaIndex != -1) delay_s = line.substr(0, commaIndex);
				else delay_s = line;

				istringstream iss(delay_s);
				iss >> delay;
				delays.push_back(delay);

				line = line.substr(commaIndex + 1);
			}
			
		}
	}

	in_anim.close();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// LOAD ANIM
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Event::LoadAnim(string animPath)
{
	Animation* anim = NULL;
	std::vector<int> delays;

	InitDelaysOfAnim("Battle/battle_standing_left", delays);
			
	anim = new Animation(string("Files/Events/") + m_name + string("/") + animPath, delays, false);

	m_animMap.insert(pair<string, Animation*>(animPath, anim));

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// DISPLAY
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void Event::Display()
{

		/*m_animMap[m_currentAnim]->Update();
		m_animMap[m_currentAnim]->GetCurrentSprite().SetPosition(m_posX, m_posY);
		App.Draw(m_animMap[m_currentAnim]->GetCurrentSprite());
		#ifdef _DEBUG
			m_animMap[m_currentAnim]->GetCurrentSprite().DisplayExtension(App);
		#endif*/
	
}

void Event::Move(float x, float y)
{
	this->SetPosition(m_posX + x, m_posY + y);
}

void Event::SetCurrentAnim(string anim)
{
	/*if(m_currentAnim.compare(anim) == 0) return;

	//===================================
	// ON STOPPE TOUTES LES ANIMATIONS SAUF CELLE DE L'ETAT COURANT
	//===================================
	std::map<string,Animation*>::iterator it;

	for(it = m_animMap.begin() ; it != m_animMap.end() ; ++it)
		if( (*it).first != anim ) m_animMap[(*it).first]->Stop();*/

}

void Event::SetPosition(float x, float y)
{
	m_posX = x;
	m_posY = y;
}

string Event::GetName() const
{
	return m_name;
}

Animation* Event::GetCurrentAnim()
{
	return m_animMap[m_currentAnim];
}

/*sf::Vector2f Event::GetPosition()
{
	return sf::Vector2f(m_posX, m_posY);
	
}*/