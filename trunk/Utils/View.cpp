#include "View.h"

View::View()
{

}

View::View(int x, int y, int w, int h)
{
	m_size.x = w;
	m_size.y = h;
	m_center.x = w - x;
	m_center.y = h - y;
}

View::View(Vector2f& center, Vector2f& size)
{
	m_center = center;
	m_size = size;
}

View::~View()
{

}

Vector2f View::GetSize()
{
	return m_size;
}

Vector2f View::GetCenter()
{
	return m_center;
}

Vector2f View::GetOrigin()
{
	Vector2f origin;
	origin.x = m_center.x - (m_size.x / 2);
	origin.y = m_center.y - (m_size.y / 2);

	return origin;
}

void View::SetCenter(Vector2f& c)
{
	m_center = c;
}

void View::SetCenter(int x,int y)
{
	m_center.x = x;
	m_center.y = y;
}

void View::SetSize(Vector2f& s)
{
	m_size = s;
}

void View::SetSize(int w, int h)
{
	m_size.x = w;
	m_size.y = h;
}