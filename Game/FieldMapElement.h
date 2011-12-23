#ifndef FIELDMAPELEMENT_H
#define FIELDMAPELEMENT_H

#include "Displayable.h"
#include "Collidable.h"
class FieldMap;


class FieldMapElement : public Collidable, public Displayable
{
	protected:
		Vector2f m_tile_pos;
		FieldMap* m_map;
		unsigned int m_tile_width;
		unsigned int m_tile_height;
		FieldMapElement() { m_tile_width = 0; m_tile_height = 0; m_map = NULL; }

	public:
		virtual Vector2f& GetTilePosition()					= 0;
		virtual FieldMap* GetFieldMap() { return m_map; }
		virtual void	  SetFieldMap(FieldMap* fm) { m_map = fm; }
		virtual unsigned int GetTileWidth() { return m_tile_width; }
		virtual void	  SetTileWidth(unsigned int w) { m_tile_width = w; }
		virtual unsigned int GetTileHeight() { return m_tile_height; }
		virtual void	  SetTileHeight(unsigned int h) { m_tile_height = h; }
};

#endif
