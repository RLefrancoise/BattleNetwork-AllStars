PROJECT(GAME)

INCLUDE_DIRECTORIES(	
			${UTILS_SOURCE_DIR}
			${SCENE_SOURCE_DIR}
			${SFML_INCLUDE_DIR}
			${BOOST_INCLUDE_DIR}
)

ADD_LIBRARY(Game_lib STATIC 

			Actor.cpp
			Actor.h
			ActorLifeBar.cpp
			ActorLifeBar.h
			Animation.hpp
			Animation.cpp
			Event.h
			Event.cpp
			ExtendedSprite.h
			ExtendedSprite.cpp
			Object.h
			Object.cpp
)
