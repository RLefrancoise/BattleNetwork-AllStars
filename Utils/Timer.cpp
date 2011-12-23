#include "Timer.h"
#include <time.h>

Timer::Timer()
{
    //Initialisation des variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //On demarre le timer
    started = true;

    //On enlËve la pause du timer
    paused = false;

    //On rÈcupÈre le temps courant
	sceRtcGetCurrentTick(&startTicks);
	//startTicks *= 1000;
}

void Timer::stop()
{
    //On stoppe le timer
    started = false;

    //On enlËve la pause
    paused = false;
}

float Timer::get_ticks()
{
    //Si le timer est en marche
    if( started == true )
    {
        //Si le timer est en pause
        if( paused == true )
        {
            //On retourne le nombre de ticks quand le timer a ÈtÅEmis en pause
            return (float) (pausedTicks / 1000.0f);
        }
        else
        {
			u64 t;
			sceRtcGetCurrentTick(&t);
			//t *= 1000;
            //On retourne le temps courant moins le temps quand il a dÈmarrÅE
            return (float) ((t - startTicks) / 1000);
			//return ( (double)clock() - startTicks ) / CLOCKS_PER_SEC;
        }
    }

    //Si le timer n'est pas en marche
    return 0;
}

void Timer::pause()
{
    //Si le timer est en marche et pas encore en pause
    if( ( started == true ) && ( paused == false ) )
    {
        //On met en pause le timer
        paused = true;

        //On calcul le pausedTicks
		u64 t;
		sceRtcGetCurrentTick(&t);
		//t *= 1000;
        pausedTicks = t - startTicks;
		//pausedTicks = ( (double)clock() - startTicks ) / CLOCKS_PER_SEC;
    }
}

void Timer::unpause()
{
    //Si le timer est en pause
    if( paused == true )
    {
        //on enlËve la pause du timer
        paused = false;

        //On remet ÅEzero le startTicks
		u64 t;
		sceRtcGetCurrentTick(&t);
		//t *= 1000;
        startTicks = t - pausedTicks;
		//startTicks = ( (double)clock() - pausedTicks ) / CLOCKS_PER_SEC;

        //Remise ÅEzero du pausedTicks
        pausedTicks = 0;
    }
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}
