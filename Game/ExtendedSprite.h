#ifndef EXTENDED_SPRITE_HPP
#define EXTENDED_SPRITE_HPP

//typedef sf::IntRect AABBCollisionBox;
#include "Utils.h"

struct AABBCollisionBox
{
	int Left;
	int Top;
	int Width;
	int Height;
};

class ExtendedSprite /*: public sf::Sprite*/
{
	public :

		ExtendedSprite();
		//ExtendedSprite(const sf::Image &image);
		~ExtendedSprite();

		void SetHotPoint(Vector2i hp);
		void SetPosition(float X, float Y);
		void SetImage(OSL_IMAGE* img);

		void AddDefenceBox(AABBCollisionBox box);
		void AddOffenseBox(AABBCollisionBox box);
		void ReverseCollisionsBoxes();

		void Display(float offX = 0, float offY = 0);
		void DisplayExtension();
		void LoadCollisions(int frameNb, std::string path);

		std::vector<AABBCollisionBox> GetDefenceBoxes() const;
		std::vector<AABBCollisionBox> GetOffenseBoxes() const;
		Vector2i GetHotPoint() const;
		Vector2i GetSize() const;

	private :

		std::vector<AABBCollisionBox> m_defenceBoxes;
		std::vector<AABBCollisionBox> m_offenseBoxes;
		Vector2i m_hotPoint;
		Shape m_colliRect;
		OSL_IMAGE* m_sprite;


};

#endif // EXTENDED_SPRITE_HPP