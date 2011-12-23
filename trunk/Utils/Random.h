#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <cstdlib>
#include <cstdio>
#include <ctime>

class Random
{
    private:
        Random();

    public:
        static int RandomInt(int min, int max);
};

#endif // RANDOM_H_INCLUDED
