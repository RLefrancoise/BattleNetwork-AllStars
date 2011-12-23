#include "ImgManager.h"
#include "Logger.h"
#include "StringUtils.h"

#include <string>

using namespace std;

void ImgManager::Reset()
{
	map<string, OSL_IMAGE*>::iterator it;
	for(it = m_imageMap.begin() ; it != m_imageMap.end() ; ++it)
		if(it->second != NULL) oslDeleteImage(it->second);

	m_imageMap.clear();
	return;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// COPY IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void ImgManager::CopyImage(OSL_IMAGE* img, OSL_IMAGE* dest)
{
	(*dest) = (*img);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// REVERSE IMG
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void ImgManager::ReverseIMG(OSL_IMAGE* Image)
{
	/*int i;
	
	sf::Uint8 *pixel1, *pixel2;

	if (&Image == NULL) return;

	// on prend chaque ligne une par une
	for (int y = 0; y < Image.GetHeight(); y++) {

		// pixel1 pointe vers le premier pixel de la ligne
		pixel1 = (sf::Uint8*) Image.GetPixelsPtr() + y * 4 * Image.GetWidth();

		// pixel2 pointe après le dernier pixel de la ligne
		pixel2 = pixel1 + Image.GetWidth() * 4;

		// on fait se rapprocher les pointeurs, et dès qu'ils se touchent (au milieu), c'est fini pour cette ilgne
		while (pixel1 < pixel2) {
		
		pixel2 -= 4;

		// on échange les valeurs *pixel1 et *pixel2 avec une petite boucle
		for (i = 0; i < 4; i++) {
			sf::Uint8 tmp = *(pixel2 + i);
			*(pixel2 + i) = *(pixel1 + i);
			*(pixel1 + i) = tmp;
		}

		pixel1 += 4;
		}
	}*/

}


std::map<std::string, OSL_IMAGE*> ImgManager::m_imageMap;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ADD IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void  ImgManager::AddImage(string name, OSL_IMAGE* Image)
{
	//image déjà présente ?
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	if(it == m_imageMap.end())
	{
		pair<string, OSL_IMAGE*> p;
		p.first = name;
		p.second = Image;
		m_imageMap.insert(p);
		LOG("Add picture : " + name);
	}

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// EXISTS
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
bool ImgManager::Exists(string name)
{
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	return (it != m_imageMap.end());
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// GET IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
OSL_IMAGE* ImgManager::GetImage(string name)
{

	//image déjà chargée ?
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	//si non
	if(it == m_imageMap.end())
	{
		//on charge l'image
		char n[name.size() + 1];
		StringUtils::StringToChar(name, n);
		OSL_IMAGE* img = oslLoadImageFilePNG(n, OSL_IN_RAM, OSL_PF_5551);

		if(!img) //si le chargement échoue on log et on quitte
		{
			LOG("Can't load picture : " + name);
			oslQuit();
		}
		else //si le chargement a réussi on stocke dans la map
		{

			pair<string, OSL_IMAGE*> p;
			p.first = name;
			p.second = img;
			m_imageMap.insert(p);
			LOG("Load picture : " + name);
		}
	}

	//finalement, on retourne l'image
	return m_imageMap[name];
}
