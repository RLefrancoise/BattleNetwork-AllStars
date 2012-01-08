#include "ImgManager.h"
#include "Logger.h"
#include "StringUtils.h"

#include <string>

using namespace std;

SceUID ImgManager::m_img_sema = -1;
bool ImgManager::m_initialized = false;

void ImgManager::Initialize()
{
	m_img_sema = sceKernelCreateSema("img_manager_sem", 0, 1, 1, NULL);
	if(m_img_sema < 0) oslQuit();
	
	m_initialized = true;
}

void ImgManager::LockSema()
{
	int ret = sceKernelWaitSema(m_img_sema, 1, 0);
	if (ret < 0) printf("sceKernelWaitSema(%08x) failed with %08x\n", m_img_sema, ret);
}

void ImgManager::UnlockSema()
{
	int ret = sceKernelSignalSema(m_img_sema, 1);
	if (ret < 0) printf("sceKernelSignalSema(%08x) failed with %08x\n", m_img_sema, ret);
}




void ImgManager::Reset()
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	map<string, OSL_IMAGE*>::iterator it;
	for(it = m_imageMap.begin() ; it != m_imageMap.end() ; ++it)
		if(it->second != NULL) oslDeleteImage(it->second);

	m_imageMap.clear();
	
	UnlockSema();
	
	return;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// COPY IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void ImgManager::CopyImage(OSL_IMAGE* img, OSL_IMAGE* dest)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	//(*dest) = (*img);
	oslCopyImageTo(dest, img);
	
	UnlockSema();
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
	if(!m_initialized) Initialize();
	
	LockSema();
	
	//image déjà présente ?
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	if(it == m_imageMap.end())
	{
		pair<string, OSL_IMAGE*> p;
		p.first = name;
		p.second = Image;
		m_imageMap.insert(p);
		
		#ifdef _DEBUG
			LOG("Add picture : " + name);
		#endif
	}
	
	UnlockSema();

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// EXISTS
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
bool ImgManager::Exists(string name)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	UnlockSema();
	
	return (it != m_imageMap.end());
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// GET IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
OSL_IMAGE* ImgManager::GetImage(string name)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	//image déjà chargée ?
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);

	//si non
	if(it == m_imageMap.end())
	{
		//on charge l'image
		char n[name.size() + 1];
		StringUtils::StringToChar(name, n);
		OSL_IMAGE* img = oslLoadImageFilePNG(n, OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_5551);

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
		#ifdef _DEBUG
			LOG("Load picture : " + name);
		#endif
		}
	}

	UnlockSema();
	
	//finalement, on retourne l'image
	return m_imageMap[name];
}

void ImgManager::RemoveImage(string name)
{
	if(!m_initialized) Initialize();
	
	LockSema();
	
	//image déjà chargée ?
	map<string, OSL_IMAGE*>::iterator it;
	it = m_imageMap.find(name);
	
	//si oui
	if(it != m_imageMap.end())
	{
		if(it->second) oslDeleteImage(it->second);
	#ifdef _DEBUG
		LOG("Remove picture : " + name);
	#endif
		
	}
	
	UnlockSema();
}
