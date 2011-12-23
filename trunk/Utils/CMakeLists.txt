PROJECT(UTILS)

INCLUDE_DIRECTORIES(	
			${SFML_INCLUDE_DIR}
			${BOOST_INCLUDE_DIR}
)

ADD_LIBRARY(Utils_lib STATIC 

			FontManager.cpp
			FontManager.h
			ImgManager.cpp
			ImgManager.h
			Logger.cpp
			Logger.h
			Random.h
			Random.cpp
			Singleton.h
			SndManager.cpp
			SndManager.h
			StringUtils.cpp
			StringUtils.h
			Timer.cpp
			Timer.h

)
