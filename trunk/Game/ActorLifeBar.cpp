#include "ActorLifeBar.h"
#include "BattleActor.h"
#include "ImgManager.h"

#include <math.h>
#include <iostream>

#define PI 3.14159265

using namespace std;

ActorLifeBar::ActorLifeBar(BattleActor* actor)
{
	m_actor = actor;
	m_life_pic = ImgManager::GetImage(string("Actors/") + actor->GetName() + string("/life_pic.png"));
	m_life_pic_damage = ImgManager::GetImage(string("Actors/") + actor->GetName() + string("/life_pic_damage.png"));
	m_life_pic_critical = ImgManager::GetImage(string("Actors/") + actor->GetName() + string("/life_pic_critical.png"));
	m_extended = true;
}

ActorLifeBar::~ActorLifeBar()
{
}

void ActorLifeBar::Display(int x, int y)
{
	
	if(!m_extended)
	{
		
		float lifePercent = ((float)m_actor->GetHP()) / ((float)m_actor->GetMaxHP());
		OSL_IMAGE* d_lifeBar = ImgManager::GetImage("Battle/NonExtendedLifeBar_d.png");
		oslSetAlpha(OSL_FX_ALPHA, 192);
		oslDrawImageXY(d_lifeBar, x, y);
		oslSetAlpha(OSL_FX_ALPHA, 255);

		OSL_IMAGE* lifeBar = ImgManager::GetImage("Battle/NonExtendedLifeBar.png");
		oslSetImageTile(lifeBar, 0, 0, lifePercent * lifeBar->sizeX, lifeBar->sizeY);
		oslSetAlpha(OSL_FX_ALPHA, 192);
		oslDrawImageXY(lifeBar, x, y);
		oslSetAlpha(OSL_FX_ALPHA, 255);

		oslDrawRect(x, y, x + d_lifeBar->sizeX, y + d_lifeBar->sizeY, RGB(0,0,0));
	}
	else
	{
		OSL_IMAGE* lifeBar = ImgManager::GetImage("Battle/ExtendedLifeBar.png");
		oslSetAlpha(OSL_FX_ALPHA, 192);
		oslDrawImageXY(lifeBar, x - 105, y - 75);
		oslSetAlpha(OSL_FX_ALPHA, 255);

		oslDrawImageXY(m_life_pic, x, y - 75);
	}
	
}

void ActorLifeBar::SetExtended(bool b)
{
	m_extended = b;
}