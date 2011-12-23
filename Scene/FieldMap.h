#ifndef FieldMap_H
#define FieldMap_H

#include "FieldMapElement.h"

//class Actor : public FieldMapElement;
//class Object : public FieldMapElement;

#include "Screens.hpp"
#include "Actor.h"
#include "Object.h"
#include "Utils.h"

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <oslib/oslib.h>
#include <queue>

typedef boost::multi_array< std::pair<int, int> , 2 >		Mapping			;
typedef boost::multi_array< int , 2 >						Collisions		;
//typedef boost::multi_array< sf::Sprite*, 2 >				Objects			;
typedef boost::multi_array< Object*, 2 >					Objects			;

typedef boost::shared_ptr<Mapping>							MappingPtr		;
typedef boost::shared_ptr<Collisions>						CollisionsPtr	;
typedef boost::shared_ptr<Objects>							ObjectsPtr		;

class FieldMap : public Screen
{

	public:

		enum ActorMoving
		{
			AM_LEFT,
			AM_RIGHT,
			AM_UP,
			AM_DOWN,
			AM_UP_LEFT,
			AM_UP_RIGHT,
			AM_DOWN_LEFT,
			AM_DOWN_RIGHT
		};

		struct Grid
		{
			Vector2f origin;
			Vector2f xVectorPoint;
			Vector2f yVectorPoint;
			Vector2f xVector;
			Vector2f yVector;
			MappingPtr firstMapping;
			CollisionsPtr collisions;
			ObjectsPtr objects;
			float tileXSize;
			float tileYSize;
			int xTilesNb;
			int yTilesNb;
			int xScroll;
			int yScroll;
			OSL_IMAGE* tileset;
		};

		//Construit la carte
		FieldMap()								;
		//Détruit la carte
		~FieldMap()							;
		//Démarre la carte
		virtual int	Run()		;
		void	Initialize()										;

		void Load(std::string file);
		void AddElement(FieldMapElement* e, float x, float y, bool catchToGrid = true, std::string id = "");

		Vector2f GetActorTile(float x, float y);

	private:
		
		static int sum(int n)
		{
			return (n*(n+1)/2);
		}

		static int depth(float x, float y)
		{
			//int r = sum(x + y) + x + 1;
			//r = ((x + y) > 29) ? r -= sum(x + y - 29) : r;
			//r = ((x + y) > 25) ? r -= sum(x + y - 25) : r;
			//return (r*10);

			int f1 = 10 * 30 * 25;
			int f2 = 300;
			int f3 = f1 + f2 / 2;
			return ( ( (x + y) * f1 ) + (x * f3) );
		}

		class DisplayableComparaison
		{
			friend class FieldMap;

			public:
				bool operator() (FieldMapElement* d1, FieldMapElement* d2) const
				{
					
					//return (depth(d1->GetPosition().x, d1->GetPosition().y) > depth(d2->GetPosition().x, d2->GetPosition().y));

					//int f1 = 10 * 30 * 25;
					//int f2 = (30 > 25) ? 30 * 10 : 25 * 10;
					//int f2 = max(m_grid.xTilesNb, m_grid.yTilesNb) * 10;
					//int f3 = f1 + f2 / 2;
					//int posd1 = ( (d1->GetPosition().x + d1->GetPosition().y) * f1 ) + ( d1->GetPosition().x * f3 );
					//int posd2 = ( (d2->GetPosition().x + d2->GetPosition().y) * f1 ) + ( d2->GetPosition().x * f3 );

					//return (posd1 > posd2);

					int x1 = d1->GetTilePosition().x;
					int y1 = d1->GetTilePosition().y;
					int x2 = d2->GetTilePosition().x;
					int y2 = d2->GetTilePosition().y;
					unsigned int _w = d2->GetTileWidth();
					unsigned int _h = d2->GetTileHeight();

					if( (_w == 0) && (_h == 0) ) //element which is only on 1 tile of the map
					{
						if( (x1 + y1) == (x2 + y2) ) //both elements on the same tile
						{
							if( x1 == x2)
							{
								float _pos1 = d1->GetPosition().y;
								float _pos2 = d2->GetPosition().y;
								return ( _pos1 > _pos2 );
							}
							else
								return (x1 > x2);
							
						}
						
						return ( (x1 + y1) > (x2 + y2) );
					}
					else //multi-tile elements
					{

						//--- same tile as origin of d2 ---
						if( (x1 + y1) == (x2 + y2) )
						{
							if( x1 == x2)
							{
								float _pos1 = d1->GetPosition().y;
								float _pos2 = d2->GetPosition().y;
								return ( _pos1 > _pos2 );
							}
							else
								return (x1 > x2);
						}

						//--- different tile as origin of d2 ---
						bool trueFound = false, enterCond = false;

						for(int i = 0 ; (i <= _w) && !trueFound ; i++)
						{
							for(int j = 0 ; (j <= _h) && !trueFound ; j++)
							{
								if( (x1 + y1) == (x2 - i + y2 - j) ) //both elements on the same tile
								{
									if( x1 == (x2 - i) )
									{
										float _pos1 = d1->GetPosition().y;
										float _pos2 = d2->GetPosition().y;
										trueFound = ( _pos1 > _pos2 );
										enterCond = true;
									}
									else
									{
										trueFound = ( x1 > (x2 - i) );
										enterCond = true;
									}
									
								}
								
								if(!enterCond) trueFound = ( (x1 + y1) > (x2 - i + y2 - j) );
								enterCond = false;
							}
						}

						return trueFound;

						/*if( (x1 + y1) == (x2 + y2) ) //both elements on the same tile
						{
							if( x1 == x2)
							{
								float _pos1 = d1->GetPosition().y;
								float _pos2 = d2->GetPosition().y;
								return ( _pos1 > _pos2 );
							}
							else
								return (x1 > x2);
						}*/
							
						
						//if( x1 > (x2 + _w) ) return true; //element 2 behind element 1
						//if( x1 > x2 ) return true; //element 2 behind element 1

						//if( ( x1 == x2 ) && ( y1 > (y2 + _h) ) ) return true;
						
						//return false;

						//return ( ( (x1 + y1) > (x2 + y2) ) && ( (x1 + y1) < (x2 + _w + y2 + _h) ) );
					}
					

					//return( (d1->GetPosition().x + d1->GetPosition().y) > (d2->GetPosition().x + d2->GetPosition().y) );
				}
		};

		//Mise à jour de la scène (appelée par Run)
		virtual int	Update()	;
		//Affichage de la scène (appelée par Run)
		virtual void Display()	;

		//gestion des évènements en relation avec le héros
		void ActorHandle()	;

		void MoveActor(Actor* actor, ActorMoving move);

		void PutActorOnTile(Displayable* actor, int x, int y);

		Grid m_grid								;

		//std::vector<Displayable*> m_displayables;
		std::vector<FieldMapElement*> m_displayables;

		//std::vector<Actor*> m_actors;			;
		//background
		//sf::Sprite	m_bg;						;
		Animation* m_bg;
		//vue
		View	m_view						;

		OSL_SOUND* m_bgm							;

		//===========================
		// ACTOR
		//===========================
		Actor* m_actor; //acteur
		
		//===========================
		// MAP
		//===========================
		Vector2f m_actorTile;
		//sf::Text m_actorTileText;

		void DisplayIsoMap();
		
		Vector2f IsoToScreen(float x, float y);
		void DrawColoredTile(float x, float y, unsigned int color);
		void DrawObject(float x, float y, Displayable* o);
		//Dessine le sprite d'une cellule de la carte
		void DrawSpriteCell(float x, float y, int spriteCol, int spriteLine);

		bool isWalkable(float x, float y);

		void InitZOrders(Vector2f& left, Vector2f& right, Vector2f& top, Vector2f& bottom);
		std::map<int, std::vector<Vector2i> > m_zOrders;
		//std::priority_queue<Displayable*, std::vector<Displayable*>, DisplayableComparaison > m_d_zOrders; //for Displayable elements
		std::priority_queue<FieldMapElement*, std::vector<FieldMapElement*>, DisplayableComparaison > m_d_zOrders; //for Displayable elements

		std::map<FieldMapElement*,std::string> m_elements_ids;
};

#endif //FieldMap_H
