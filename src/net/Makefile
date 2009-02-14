TARGET = nettest
OBJS = main.o ../base64.o ../tri/triNet.o ../tri/triLog.o ../tri/triMemory.o ../tri/triInput.o netextended.o 

PSPBIN = $(shell psp-config --psp-prefix)

INCDIR = 
CFLAGS = -O0 -G0 -Wall -g -DDEBUG -D_DEBUG -D_DEBUG_LOG -D_DEBUG_MEMORY -D__PSP__ $(shell $(PSPBIN)/bin/freetype-config --cflags)
#-DTRI_SUPPORT_PNG 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lstdc++ -lpspwlan

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

debug: all
	cp nettest.prx ../../

