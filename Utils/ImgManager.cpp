#include "ImgManager.h"
#include "Logger.h"
#include "StringUtils.h"

#include <string>

using namespace std;

SceUID ImgManager::m_img_sema = -1;
bool ImgManager::m_initialized = false;

//stack<vector<string> > ImgManager::m_img_stack;
map<string, vector<string> > ImgManager::m_context_map;

//string ImgManager::m_current_context;
//string ImgManager::m_previous_context;

stack<string> ImgManager::m_context_stack;

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
	#ifdef _DEBUG
	LOG("Reset ImgManager")
	#endif
	
	if(!m_initialized) Initialize();
	
	LockSema();
	
	map<string, vector<string> >::iterator it;
	for(it = m_context_map.begin() ; it != m_context_map.end() ; ++it)
	{
		vector<string> v = it->second;
		UseContext(it->first);
		vector<string>::iterator it_v;
		for(it_v = v.begin() ; it_v != v.end() ; it_v++)
			RemoveImage(*it_v);
		UsePreviousContext();
	}
	
	/*while(!m_context_stack.empty())
	{
		DestroyContext(m_context_stack.top());
		m_context_stack.pop();
	}*/
	
	m_context_map.clear();
	
	while(!m_context_stack.empty()) m_context_stack.pop();
	
	if(m_imageMap.size() != 0)
	{
		ostringstream oss(ostringstream::out);
		oss << "{ImgManager->Reset()} Memory Leaks detected ! " << m_imageMap.size() << " pictures have not been deleted. Deletion will be done without context.";
		LOG(oss.str())
		
		map<string, Image>::iterator it;
		for(it = m_imageMap.begin() ; it != m_imageMap.end() ; ++it)
			if(it->second.picture != NULL) oslDeleteImage(it->second.picture);

		m_imageMap.clear();
	}
	
	/*map<string, Image>::iterator it;
	for(it = m_imageMap.begin() ; it != m_imageMap.end() ; ++it)
		if(it->second.picture != NULL) oslDeleteImage(it->second.picture);

	m_imageMap.clear();
	
	m_context_map.clear();*/
	
	/*map<string, vector<string> >::iterator it;
	for(it = m_context_map.begin() ; it != m_context_map.end() ; ++it)
	{
		DestroyContext(it->first);
	}*/
	
	/*while(ExistsContext())
	{
		DestroyContext();
	}*/
	
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


std::map<std::string, ImgManager::Image> ImgManager::m_imageMap;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// ADD IMAGE
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
void  ImgManager::AddImage(string name, OSL_IMAGE* i)
{
	if(!m_initialized) Initialize();
	
	//if(!ExistsContext(m_current_context))
	if(m_context_stack.empty())
	{	
		LOG("No existing context for ImgManager : can't add the required image")
		oslQuit();
	}	
	
	LockSema();
	
	//image déjà présente ?
	map<string, ImgManager::Image>::iterator it;
	it = m_imageMap.find(name);

	if(it == m_imageMap.end())
	{
		pair<string, ImgManager::Image> p;
		p.first = name;
		ImgManager::Image im = { i, 1 };
		p.second = im;
		m_imageMap.insert(p);
		
		if(!ExistsInContext(name)) m_context_map[m_context_stack.top()].push_back(name);
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
	
	map<string, ImgManager::Image>::iterator it;
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
	
	//if(!ExistsContext(m_current_context))
	if(m_context_stack.empty())
	{	
		LOG("No existing context for ImgManager : can't get the required image")
		oslQuit();
	}	
	
	LockSema();
	
	//image déjà chargée ?
	map<string, ImgManager::Image>::iterator it;
	it = m_imageMap.find(name);

	//si non
	if(it == m_imageMap.end())
	{
		//on charge l'image
		char n[name.size() + 1];
		StringUtils::StringToChar(name, n);
		//OSL_IMAGE* img = oslLoadImageFilePNG(n, OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_5551);
		OSL_IMAGE* img = oslLoadImageFilePNG(n, OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_8888);
		
		if(!img) //si le chargement échoue on log et on quitte
		{
			LOG("Can't load picture : " + name);
			oslQuit();
		}
		else //si le chargement a réussi on stocke dans la map
		{

			pair<string, ImgManager::Image> p;
			p.first = name;
			Image i = { img, 1};
			p.second = i;
			m_imageMap.insert(p);
			
			if(!ExistsInContext(name)) m_context_map[m_context_stack.top()].push_back(name);
			
			//m_img_stack.top().push_back(name); //add loaded image to current context
			
		#ifdef _DEBUG
			LOG("Load picture : " + name);
		#endif
		}
	}
	//si oui, on regarde si elle a été demandé dans le contexte actuel
	else
	{
		//si elle n'existe pas, on l'ajoute
		if(!ExistsInContext(name))
		{
			m_imageMap[name].ref_cpt++;
			m_context_map[m_context_stack.top()].push_back(name);
			//m_img_stack.top().push_back(name);
		}
	}
	
	UnlockSema();
	
	#ifdef _DEBUG
		ostringstream oss(ostringstream::out);
		oss << "Get picture : " << name << " [ref: " << m_imageMap[name].ref_cpt << "] [Context:" << m_context_stack.top() << "]";
		LOG(oss.str());
	#endif
		
	//finalement, on retourne l'image
	return m_imageMap[name].picture;
}

void ImgManager::RemoveImage(string name)
{
	if(!m_initialized) Initialize();
	
	//if(!ExistsContext(m_current_context))
	if(m_context_stack.empty())
	{	
		LOG("No existing context for ImgManager : can't remove the required image")
		oslQuit();
	}	
	
	//LockSema();
	
	//image déjà chargée ?
	map<string, ImgManager::Image>::iterator it;
	it = m_imageMap.find(name);
	
	//si oui
	if(it != m_imageMap.end())
	{
		if(it->second.ref_cpt == 1)
		{
			if(it->second.picture) oslDeleteImage(it->second.picture);
			m_imageMap.erase(it);
			#ifdef _DEBUG
				LOG("Remove picture : " + name + "[Context:" + m_context_stack.top() + "]");
			#endif
		}
		else
		{
			it->second.ref_cpt--;
			#ifdef _DEBUG
				LOG("Decrease picture reference: " + name + "[Context:" + m_context_stack.top() + "]");
			#endif
		}
		
		RemoveFromContext(name);	
		
	}
	
	//UnlockSema();
}

void ImgManager::CreateContext(string name)
{
	#ifdef _DEBUG
		LOG("Create ImgManager Context: " + name)
	#endif
	
	if(!m_initialized) Initialize();
	
	LockSema();
	
	vector<string> v;
	//m_img_stack.push(v);
	if(ExistsContext(name)) DestroyContext(name);
	
	m_context_map[name] = v;
	
	//UseContext(name);
	
	UnlockSema();
}

void ImgManager::DestroyContext(string name)
{
	#ifdef _DEBUG
		LOG("Destroy ImgManager Context: " + name)
	#endif
	
	if(!m_initialized) Initialize();
	
	LockSema();
	
	UseContext(name);
	
	vector<string>::iterator it;
	for(it = m_context_map[name].begin() ; it != m_context_map[name].end() ; ++it)
	//for(it = m_img_stack.top().begin() ; it != m_img_stack.top().end() ; ++it)
	{
		RemoveImage(*it);
		--it;
	}
	
	//m_img_stack.pop();
	m_context_map.erase(name);
	
	UsePreviousContext();
	
	UnlockSema();
}

inline bool ImgManager::ExistsContext(string name)
{
	map<string, vector<string> >::iterator it;
	it = m_context_map.find(name);
	
	return it != m_context_map.end();
}

bool ImgManager::ExistsInContext(std::string name)
{
	if(!m_initialized) Initialize();
	
	//if(!ExistsContext(m_current_context))
	if(m_context_stack.empty())
	{	
		LOG("No existing context for ImgManager : ExistsInCurrentContext can't work")
		oslQuit();
	}	
	
	//LockSema();
	
	bool found = false;
	
	vector<string>::iterator it;
	for(it = m_context_map[m_context_stack.top()].begin() ; it != m_context_map[m_context_stack.top()].end() ; ++it)
	//for(it = m_img_stack.top().begin() ; !found && (it != m_img_stack.top().end()) ; ++it)
	{
		if(it->compare(name) == 0)
			found = true;
	}

	//UnlockSema();
	
	return found;
}

void ImgManager::RemoveFromContext(std::string name)
{
	if(!m_initialized) Initialize();
	
	//if(!ExistsContext(m_current_context))
	if(m_context_stack.empty())
	{	
		LOG("No existing context for ImgManager : can't remove picture from current context")
		oslQuit();
	}	
	
	//LockSema();
	
	if(!ExistsInContext(name)) return;
	
	vector<string>::iterator it;
	for(it = m_context_map[m_context_stack.top()].begin() ; it != m_context_map[m_context_stack.top()].end() ; ++it)
	//for(it = m_img_stack.top().begin() ; it != m_img_stack.top().end() ; ++it)
	{
		if(it->compare(name) == 0)
		{
			m_context_map[m_context_stack.top()].erase(it);
			//m_img_stack.top().erase(it);
			break;
		}
	}
	
	//UnlockSema();
}

void ImgManager::UseContext(string name)
{
	if(!ExistsContext(name)) CreateContext(name);
	
	//m_context_stack.push(m_current_context);
	
	//m_previous_context = m_current_context;
	//m_current_context = name;
	
	m_context_stack.push(name);
	
	#ifdef _DEBUG
	LOG("Use context: " + name)
	#endif
}

void ImgManager::UsePreviousContext()
{
	if(!m_context_stack.empty())
	{
		//m_current_context = m_context_stack.top();
		m_context_stack.pop();
	}
	//m_current_context = m_previous_context;
	//m_previous_context = m_current_context;
	
	#ifdef _DEBUG
	LOG("Use previous context")
	#endif
}
		