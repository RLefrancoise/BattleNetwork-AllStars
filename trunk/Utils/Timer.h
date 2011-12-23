#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <pspkernel.h>
#include <psprtc.h>

class Timer
{
    private:
        //Le temps quand le timer est lanc�
        u64 startTicks;

        //Le temps enregistr� quand le Timer a �t� mis en pause
        u64 pausedTicks;

        //Le status du Timer
        bool paused;
        bool started;

		//sf::Clock m_clock;

    public:
        //Initialise les variables
        Timer();

        //Les diff�rentes actions du timer
        void start();
        void stop();
        void pause();
        void unpause();

        //recup�re le nombre de millisecondes depuis que le timer a �t� lanc�
        //ou r�cup�re le nombre de millisecondes depuis que le timer a �t� mis en pause
        float get_ticks();

        //Fonctions de v�rification du status du timer
        bool is_started();
        bool is_paused();
};

#endif // TIMER_H_INCLUDED
