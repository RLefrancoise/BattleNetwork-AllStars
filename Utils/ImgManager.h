#ifndef IMGMANAGER_H_INCLUDED
#define IMGMANAGER_H_INCLUDED

#include <string>
#include <map>

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

	private:
		
		static std::map<std::string, OSL_IMAGE*> m_imageMap;
		ImgManager();
		virtual ~ImgManager() = 0;
		
		static SceUID m_img_sema		;
		static bool	m_initialized		;
		
		static void Initialize()		;
		static void LockSema()			;
		static void UnlockSema()		;
};

#endif
