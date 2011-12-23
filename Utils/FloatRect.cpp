#include "FloatRect.h"

FloatRect::FloatRect()
{

}

FloatRect::FloatRect(float left, float top, float width, float height)
{
	m_position.x = left;
	m_position.y = top;
	m_size.x = width;
	m_size.y = height;
}

FloatRect::FloatRect(const Vector2f& position, const Vector2f& size)
{
	m_position = position;
	m_size = size;
}

bool FloatRect::Contains(float x, float y)
{
	if( (x >= m_position.x) && (x <= m_position.x + m_size.x) && (y >= m_position.y) && (y <= m_position.y + m_size.y) ) return true;

	return false;
}

bool FloatRect::Contains(const Vector2f& p)
{
	if( (p.x >= m_position.x) && (p.x <= m_position.x + m_size.x) && (p.y >= m_position.y) && (p.y <= m_position.y + m_size.y) ) return true;

	return false;
}

bool FloatRect::Intersects(const FloatRect& rect)
{
	if((rect.m_position.x >= m_position.x + m_size.x)		// trop à droite
	|| (rect.m_position.x + rect.m_size.x <= m_position.x)	// trop à gauche
	|| (rect.m_position.y >= m_position.y + m_size.y)		// trop en bas
	|| (rect.m_position.y + rect.m_size.y <= m_position.y)) // trop en haut
          return false; 
   else
          return true; 
}