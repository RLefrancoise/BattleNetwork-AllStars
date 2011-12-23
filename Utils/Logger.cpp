#include "Logger.h"

#include <psputils.h>

using namespace std;

void Logger::Write(string msg)
{
    struct tm Today;
	time_t now;
	//timeval tv;
	//timezone tz;
	
	//sceKernelLibcTime(&now);
	time(&now);
	Today = *localtime(&now);
	//sceKernelLibcGettimeofday(&tv, &tz);
	//Today = *localtime(&(tv.tv_sec));

    ostringstream oss;
	oss << "[" << Today.tm_year + 1900 << "/" << Today.tm_mon + 1 << "/" << Today.tm_mday << " " << (Today.tm_hour +2) << ":" << Today.tm_min << ":" << Today.tm_sec << "]" << msg.c_str() << "\r\n";

	ofstream out("Log.txt", ofstream::app);
	if(out.good()) out << oss.str().c_str();
	else fprintf(stderr, "can't open Log.txt");
	out.close();
}
