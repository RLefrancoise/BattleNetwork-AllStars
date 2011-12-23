#ifndef DISPLAYABLE_H
#define DISPLAYABLE_H

#include "Utils.h"

class Displayable
{
	public :
		virtual void Display(float offX = 0, float offY = 0)	= 0;
		virtual void Move(float x, float y)						= 0;
		virtual void SetPosition(float x, float y)				= 0;
		virtual Vector2f& GetPosition()							= 0;
};

#endif
