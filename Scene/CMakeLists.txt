PROJECT(SCENE)

INCLUDE_DIRECTORIES(	
			${UTILS_SOURCE_DIR}
			${GAME_SOURCE_DIR}
			${SFML_INCLUDE_DIR}
			${BOOST_INCLUDE_DIR}
)

ADD_LIBRARY(Scene_lib STATIC 

			BattleMap.cpp
			BattleMap.h
			FieldMap.cpp
			FieldMap.h
			Screen.hpp
			Screens.hpp
			Title.cpp
			Title.h
)
