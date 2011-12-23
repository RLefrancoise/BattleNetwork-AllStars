#ifndef VIEW_H
#define VIEW_H

#include "Vector2f.h"

class View
{
	public:
		View();
		View(int x, int y, int w, int h);
		View(Vector2f& center, Vector2f& size);
		~View();
		Vector2f GetSize();
		Vector2f GetCenter();
		Vector2f GetOrigin();
		void SetCenter(Vector2f& c);
		void SetCenter(int x,int y);
		void SetSize(Vector2f& s);
		void SetSize(int w, int h);

	private:
		Vector2f m_size;
		Vector2f m_center;


};

#endif
