#include "Shape.h"

Shape::Shape()
{

}

Shape::~Shape()
{

}

int Shape::GetPointsCount() const
{
	return m_points.size();
}

Vector2f Shape::GetPointPosition(unsigned int i)
{
	return m_points[i];
}

Vector2f Shape::GetPosition()
{
	return m_position;
}

void Shape::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

void Shape::SetPosition(Vector2f& v)
{
	m_position = v;
}

void Shape::AddPoint(Vector2f p)
{
	m_points.push_back(p);
}

void Shape::AddPoint(float x, float y)
{
	m_points.push_back(Vector2f(x,y));
}

void Shape::Move(float offX, float offY)
{
	this->SetPosition(m_position.x + offX, m_position.y + offY);
}

void Shape::Display(float offX, float offY)
{
	for(unsigned int i = 0 ; i < m_points.size() ; ++i)
	{
		if(i == m_points.size() - 1) oslDrawLine(m_position.x + m_points[i].x + offX, m_position.y + m_points[i].y + offY, m_position.x + m_points[0].x + offX, m_position.y + m_points[0].y + offY, m_color);
		else oslDrawLine(m_position.x + m_points[i].x + offX, m_position.y + m_points[i].y + offY, m_position.x + m_points[i+1].x + offX, m_position.y + m_points[i+1].y + offY, m_color);
	}
}

void Shape::SetColor(OSL_COLOR color)
{
	m_color = color;
}