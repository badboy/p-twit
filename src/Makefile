TARGET = psptwitter
OBJS = tri/triLog.o main.o base64.o init.o options_menu.o timeline.o timeline_menu.o netextended.o twitter.o \
osk.o CConfig.o helpers.o datehelpers.o timeline.o UriCodec.o \
tri/streams/streams.o tri/triVAlloc.o tri/triMemory.o tri/triRefcount.o tri/triNet.o \
tri/triImage.o tri/rle.o tri/triGraphics.o tri/triFont.o tri/triInput.o \
tinyxml/tinystr.o tinyxml/tinyxml.o tinyxml/tinyxmlerror.o tinyxml/tinyxmlparser.o \

PSPBIN = $(shell psp-config --psp-prefix)

INCDIR = 
CFLAGS = -O0 -G0 -Wall -g -DDEBUG -D_DEBUG -D_DEBUG_LOG -DTRI_SUPPORT_PNG -D_DEBUG_MEMORY -D__PSP__ $(shell $(PSPBIN)/bin/freetype-config --cflags)
#-DTRI_SUPPORT_PNG 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lstdc++ -lpspgum -lpspgu -lpsputility -lpspwlan -lpsprtc -lpng -lfreetype -lm -lz $(shell $(PSPBIN)/bin/freetype-config --libs)

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = p-twit!
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

debug: all
	cp psptwitter.prx ../
release: all
	mkdir -p ../PSP/GAME/p-twit/
	cp EBOOT.PBP ../PSP/GAME/p-twit/
	cp ../defaultconfig.xml ../PSP/GAME/p-twit/config.xml
	cp -r ../font/ ../PSP/GAME/p-twit/
	cp -r ../img/ ../PSP/GAME/p-twit/
