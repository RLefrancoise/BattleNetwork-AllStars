#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <stack>

#include "Utils.h"

#include "Screen.hpp"

class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

	public:

		ScreenPtr getCurrentScreen() const;
        void pushScreen(ScreenPtr& state);
        void setScreen(ScreenPtr& state);
        void popScreen();

	protected:

		SceneManager();
		~SceneManager();

		std::stack<ScreenPtr> m_screens;
		
};

#endif
