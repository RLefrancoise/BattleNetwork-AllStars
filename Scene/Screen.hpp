#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <pspthreadman.h>
#include <boost/shared_ptr.hpp>

class Screen
{
	private:
		virtual int		Update()	= 0;
		virtual void	Display()	= 0;

	public:
		virtual int		Run()		= 0;
		virtual void	Initialize() = 0;
		virtual void	Destroy() = 0;
		
};

typedef boost::shared_ptr<Screen>							ScreenPtr		;

#endif
