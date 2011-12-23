#include "Variables.h"

float Variables::fps;
View Variables::m_defaultView;

float Variables::GetFPS()
{
	return fps;
}

void Variables::SetFPS(float f)
{
	fps = f;
}

View& Variables::GetDefaultView()
{
	return m_defaultView;
}