#include "Object.h"
#include "FieldMap.h"

Object::Object() : FieldMapElement()
{
	m_sprite = NULL;
}

Object::~Object()
{
	//if(m_sprite) oslDeleteImage(m_sprite);
}

OSL_IMAGE* Object::GetSprite()
{
	return m_sprite;
	
}

void Object::SetSprite(OSL_IMAGE* sprite)
{
	if(m_sprite) oslDeleteImage(m_sprite);

	m_sprite = sprite;
}

void Object::SetOrigin(float x, float y)
{
	m_origin.x = x;
	m_origin.y = y;
}

Vector2f& Object::GetOrigin()
{
	return m_origin;
}

const Vector2f& Object::GetPosition() const
{
	//m_abs_position.x = m_position.x + m_origin.x;
	//m_abs_position.y = m_position.y + m_origin.y;
	return m_abs_position;
}

Vector2f& Object::GetTilePosition()
{
	if(m_map == NULL)
	{
		m_tile_pos.x = 0;
		m_tile_pos.y = 0;
	}
	else
		m_tile_pos = m_map->GetActorTile(GetPosition().x, GetPosition().y);
	
	return m_tile_pos;
}

Vector2f& Object::GetOffset()
{
	return m_offset;
}

void Object::SetPosition(float x, float y)
{
	m_position.x = x - m_origin.x;
	m_position.y = y - m_origin.y;

	m_sprite->x = x - m_origin.x;
	m_sprite->y = y - m_origin.y;

	m_colliShape.SetPosition(x - m_origin.x, y - m_origin.y);
	
	
	m_abs_position.x = m_position.x + m_origin.x;
	m_abs_position.y = m_position.y + m_origin.y;
}

void Object::SetOffset(float offX, float offY)
{
	m_offset.x = offX;
	m_offset.y = offY;
}

Shape& Object::GetColliShape()
{
	return m_colliShape;
}

void Object::Display(float offX, float offY)
{
	//m_sprite->x += offX;
	//m_sprite->y += offY;
	//m_sprite->x += m_offset.x;
	//m_sprite->y += m_offset.y;
	oslDrawImageXY(m_sprite, m_position.x + offX, m_position.y + offY);

	//m_colliShape.Display(m_offset.x, m_offset.y);
	m_colliShape.Display(offX, offY);

	oslDrawFillRect( m_sprite->x + m_origin.x - 1, m_sprite->y + m_origin.y - 1, m_sprite->x + m_origin.x + 1, m_sprite->y + m_origin.y + 1, RGB(255,0,0) );
}

void Object::Move(float x, float y)
{
	SetPosition(m_position.x + x, m_position.y + y);
}


bool Object::Collide(Collidable* o)
{
	int i, j;
	Vector2f A,B,I,P;

	//on construit les segments de l'objet
	for(i=0 ; i < o->GetColliShape().GetPointsCount() ; i++)
	{
		A = o->GetColliShape().GetPointPosition(i);
		A.x += o->GetColliShape().GetPosition().x;
		A.y += o->GetColliShape().GetPosition().y;

		if (i == o->GetColliShape().GetPointsCount() - 1)  // si c'est le dernier point, on relie au premier
		{
			B = o->GetColliShape().GetPointPosition(0);
			B.x += o->GetColliShape().GetPosition().x;
			B.y += o->GetColliShape().GetPosition().y;
		}
		else           // sinon on relie au suivant.
		{
			B = o->GetColliShape().GetPointPosition(i+1);
			B.x += o->GetColliShape().GetPosition().x;
			B.y += o->GetColliShape().GetPosition().y;
		}

		//on construit les segments du perso
		for(j = 0 ; j < m_colliShape.GetPointsCount() ; j++)
		{
			I = m_colliShape.GetPointPosition(j);
			I.x += (m_colliShape.GetPosition().x);
			I.y += (m_colliShape.GetPosition().y);

			if (j == m_colliShape.GetPointsCount() - 1)  // si c'est le dernier point, on relie au premier
			{
				P = m_colliShape.GetPointPosition(0);
				P.x += (m_colliShape.GetPosition().x);
				P.y += (m_colliShape.GetPosition().y);
			}
			else           // sinon on relie au suivant.
			{
				P = m_colliShape.GetPointPosition(j+1);
				P.x += (m_colliShape.GetPosition().x);
				P.y += (m_colliShape.GetPosition().y);
			}


			int iseg = IntersectSegments(A,B,I,P);
			if(iseg == 1) return true;
		}
	}

	return false;
}