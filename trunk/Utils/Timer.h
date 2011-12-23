#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <pspkernel.h>
#include <psprtc.h>

class Timer
{
    private:
        //Le temps quand le timer est lancé
        u64 startTicks;

        //Le temps enregistré quand le Timer a été mis en pause
        u64 pausedTicks;

        //Le status du Timer
        bool paused;
        bool started;

		//sf::Clock m_clock;

    public:
        //Initialise les variables
        Timer();

        //Les différentes actions du timer
        void start();
        void stop();
        void pause();
        void unpause();

        //recupére le nombre de millisecondes depuis que le timer a été lancé
        //ou récupére le nombre de millisecondes depuis que le timer a été mis en pause
        float get_ticks();

        //Fonctions de vérification du status du timer
        bool is_started();
        bool is_paused();
};

#endif // TIMER_H_INCLUDED
