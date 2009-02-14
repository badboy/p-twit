CC=g++
INCLUDE=.
CCFLAGS=-g -Wall
LIBS= 
EXECUTABLE=parsexml

OBJS = tinystr.o tinyxml.o tinyxmlparser.o tinyxmlerror.o main.o 
$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(CCFLAGS) -o $@

clean:
	rm -f $(EXECUTABLE) *.o
.cpp.o:
	$(CC) -I$(INCLUDE) $(CCFLAGS) -c $<  
