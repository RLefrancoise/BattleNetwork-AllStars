#ifndef OBJECT_H
#define OBJECT_H

#include <oslib/oslib.h>
#include "Utils.h"
#include "FieldMapElement.h"

class Object : public FieldMapElement
{

	public:

		Object()											;
		~Object()											;

		OSL_IMAGE* GetSprite()								;
		void SetSprite(OSL_IMAGE* sprite)					;
		void SetOrigin(float x, float y)					;
		Vector2f& GetOrigin()								;
		Vector2f& GetPosition()								;
		Vector2f& GetTilePosition()							;
		Vector2f& GetOffset()								;
		void SetPosition(float x, float y)					;
		void SetOffset(float offX, float offY)				;
		Shape& GetColliShape()								;

		void Display(float offX = 0, float offY = 0)		;
		void Move(float x, float y)							;
		
		bool Collide(Collidable* o)							;

	protected:

		OSL_IMAGE* m_sprite;
		Shape m_colliShape;
		Vector2f m_origin;
		Vector2f m_position;
		Vector2f m_abs_position;
		Vector2f m_offset;

};

#endif //OBJECT_H
