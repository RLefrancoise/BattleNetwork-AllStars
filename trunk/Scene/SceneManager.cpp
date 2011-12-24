#include "SceneManager.h"

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
    for(unsigned int i = 0 ; i < m_screens.size() ; ++i)
    {
        popScreen();
    }
}

ScreenPtr SceneManager::getCurrentScreen() const
{
	ScreenPtr ptr;
    return ( !m_screens.empty() ) ? m_screens.top() : ptr;
}

// ===========================================================================================
/// Add a new state (become the current one)
// ===========================================================================================
void SceneManager::pushScreen( ScreenPtr& screen )
{
    // set current state
    m_screens.push( screen );
    m_screens.top()->Initialize();
}

// ===========================================================================================
/// Set the current state
// ===========================================================================================
void SceneManager::setScreen( ScreenPtr& screen )
{
    // Delete the actual current state (if any)
    popScreen ();

    // Add the new state
    pushScreen( screen );
}

// ===========================================================================================
/// Delete the current state
// ===========================================================================================
void SceneManager::popScreen()
{
    if ( !m_screens.empty() )
    {
        m_screens.pop();
    }
}
