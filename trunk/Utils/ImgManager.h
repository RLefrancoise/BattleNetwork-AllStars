#ifndef IMGMANAGER_H_INCLUDED
#define IMGMANAGER_H_INCLUDED

#include <string>
#include <map>
#include <stack>
#include <vector>

#include <oslib/oslib.h>

class ImgManager
{

	public:
		static void Reset();
		static void CopyImage(OSL_IMAGE* img, OSL_IMAGE* dest);
		static void ReverseIMG(OSL_IMAGE* Image);
		static bool Exists(std::string name);
		static void AddImage(std::string name, OSL_IMAGE* Image);
		static OSL_IMAGE* GetImage(std::string name);
		
		
		static void CreateContext(std::string name);
		static void DestroyContext(std::string name);
		static void UseContext(std::string name);
		static void UsePreviousContext();
		
	private:
		
		static void RemoveImage(std::string name);
		
		static bool ExistsContext(std::string name);
		static bool ExistsInContext(std::string name);
		static void RemoveFromContext(std::string name);
		
		struct Image
		{
			OSL_IMAGE* picture;
			unsigned int ref_cpt;
		};
		
		//static std::map<std::string, OSL_IMAGE*> m_imageMap;
		static std::map<std::string, Image> m_imageMap;
		
		static std::map<std::string, std::vector<std::string> > m_context_map;
		
		static std::string m_current_context;
		static std::string m_previous_context;
		
		static std::stack<std::string> m_context_stack;
		
		//static std::stack<std::vector<std::string> > m_img_stack;
		
		ImgManager();
		virtual ~ImgManager() = 0;
		
		static SceUID m_img_sema		;
		static bool	m_initialized		;
		
		static void Initialize()		;
		static void LockSema()			;
		static void UnlockSema()		;
};

#endif
