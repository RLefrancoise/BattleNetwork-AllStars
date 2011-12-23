#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <oslib/oslib.h>

#include "Vector2f.h"

class Shape
{

	public:
		Shape();
		~Shape();
		int GetPointsCount() const;
		Vector2f GetPointPosition(unsigned int i);
		Vector2f GetPosition();
		void SetPosition(float x, float y);
		void SetPosition(Vector2f& v);
		void AddPoint(Vector2f p);
		void AddPoint(float x, float y);
		void Move(float offX, float offY);
		void Display(float offX = 0, float offY = 0);
		void SetColor(OSL_COLOR color);

	private:
		std::vector<Vector2f> m_points;
		Vector2f m_position;
		OSL_COLOR m_color;
};

#endif
