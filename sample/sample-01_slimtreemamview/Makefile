# Makefile for GCC
#
# Author: Fabio Jun Takada Chino
#
CC=gcc
CFLAGS=-D__stMAMVIEW__
INCLUDEPATH=../../include
LIBPATH=-L../../lib
INCLUDE=-I$(INCLUDEPATH)
LIBS=-lstdc++ -lm -larboretum
SRC= app.cpp mainTest.cpp
OBJS=$(subst .cpp,.o,$(SRC))


# Implicit Rules
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

sample1: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o sample1 $(INCLUDE) $(LIBPATH) $(LIBS)

clean:
	-rm sample1 *.o
