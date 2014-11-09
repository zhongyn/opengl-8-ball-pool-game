OBJS = pool.o poolgame.o shader_utils.o
CC = g++
LDLIBS=-lglut -lSOIL -lGLEW -lGL -lm -lstdc++ 
DEBUG = -g
CFLAGS = -Wall -c -std=c++0x $(DEBUG)
LFLAGS = -Wall $(DEBUG)

pool: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o pool $(LDLIBS)

pool.o: pool.cpp shader_utils.h poolgame.h
	$(CC) $(CFLAGS) pool.cpp
	

shader_utils.o: shader_utils.cpp shader_utils.h
	$(CC) $(CFLAGS) shader_utils.cpp

poolgame.o: poolgame.cpp poolgame.h
	$(CC) $(CFLAGS) poolgame.cpp


clean:
	\rm *.o *~ pool
