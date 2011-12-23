#include "Collidable.h"

int Collidable::IntersectSegments(Vector2f A, Vector2f B, Vector2f I, Vector2f P)
{
	Vector2f D,E;
	D.x = B.x - A.x;
	D.y = B.y - A.y;
	E.x = P.x - I.x;
	E.y = P.y - I.y;
	float t = 0, u = 0;
	double denom = D.x*E.y - D.y*E.x;
	if (denom==0)
		return -1;   // erreur, cas limite
	t = - (A.x*E.y-I.x*E.y-E.x*A.y+E.x*I.y) / denom;
	if (t<0 || t>=1)
		return 0;
	u = - (-D.x*A.y+D.x*I.y+D.y*A.x-D.y*I.x) / denom;
	if (u<0 || u>=1)
		return 0;
	return 1;
}