#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "Utils.h"

class Collidable
{
	public:
		virtual Shape& GetColliShape()				= 0;
		virtual bool Collide(Collidable* o)			= 0;

	protected:
		int IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P);
};

#endif
