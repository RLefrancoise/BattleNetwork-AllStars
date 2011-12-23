#ifndef FLOATRECT_H
#define FLOATRECT_H

#include "Vector2f.h"

class FloatRect
{
	public:
		FloatRect();
		FloatRect(float left, float top, float width, float height);
		FloatRect(const Vector2f& position, const Vector2f& size);
		bool Contains(float x, float y);
		bool Contains(const Vector2f& p);
		bool Intersects(const FloatRect& rect);

	private:
		Vector2f m_position;
		Vector2f m_size;
};

#endif
