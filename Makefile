TARGET = KH_PSP
#SRC = $(wildcard Game/*.cpp) $(wildcard Scene/*.cpp) $(wildcard Utils/*.cpp) $(wildcard PRX/*.cpp) $(wildcard *.cpp)
#PRX = $(wildcard PRX/*.S)
#PRX_OBJS = $(PRX:.S=.o)
#OBJS = $(SRC:.cpp=.o) $(PRX_OBJS)

SRC = $(wildcard Game/*.cpp) $(wildcard Scene/*.cpp) $(wildcard Utils/*.cpp) $(wildcard *.cpp)
#PRX = $(wildcard PRX/*.S)
#PRX_OBJS = $(PRX:.S=.o)
OBJS = $(SRC:.cpp=.o)

CURRENTDIR = C:\Users\Renaud\Documents\programmation\C++\SVN\KHBN\khbn-code-0

INCDIR = $(CURRENTDIR)/Game $(CURRENTDIR)/Scene $(CURRENTDIR)/Utils $(CURRENTDIR)/PRX C:\Users\Renaud\Documents\programmation\C++\CMake\KH\thirdParty\boost\include
CFLAGS = -D_DEBUG -G4 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = -G1
STDLIBS= -losl -lpng -ljpeg -lz \
         -lpsphprm -lpspsdk -lpspctrl -lpspumd -lpsprtc -lpsppower -lpspgu -lpspgum  -lpspaudiolib -lpspaudio -lpsphttp -lpspssl -lpspwlan \
         -lpspnet_adhocmatching -lpspnet_adhoc -lpspnet_adhocctl -lm -lstdc++ -lsupc++ -lgcc
     
LIBS=$(STDLIBS) $(YOURLIBS)

EXTRA_TARGETS = EBOOT.PBP

# TODO: change the title of your project
PSP_EBOOT_TITLE = KH_PSP

# TODO: replace with your icon
PSP_EBOOT_ICON = ICON0.png

PSP_EBOOT_UNKPNG    = PIC0.png

PSP_EBOOT_PIC1    = PIC1.png

PSP_EBOOT_SND0    = SND0.at3

PSP_LARGE_MEMORY = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
	