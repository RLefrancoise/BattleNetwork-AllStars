#include "Random.h"

int Random::RandomInt(int min, int max)
{
	return rand() % (max - min) + min;
}
