#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include "Singleton.h"

#define LOG(msg)	Logger::Write(msg);

class Logger
{

    protected:

        Logger()						;
        virtual ~Logger() = 0			;

    public:

        static void Write(std::string msg)	;
};

#endif
