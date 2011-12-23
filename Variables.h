#ifndef VARIABLES_H
#define VARIABLES_H

#include "Utils.h"

class Variables
{
	public:
		static float GetFPS();
		static void SetFPS(float f);
		static View& GetDefaultView();

	private:
		Variables();
		virtual ~Variables() = 0;

		static float fps;
		static View m_defaultView;
};

#endif
