# Makefile for GCC
#
# Author: Fabio Jun Takada Chino
#
INCLUDEPATH=../../include
LIBPATH=-L../..
INCLUDE=-I$(INCLUDEPATH)
LIBS=-lstdc++ -lm -larboretum
SRC= app.cpp mainTest.cpp
OBJS=$(subst .cpp,.o,$(SRC))

# Implicit Rules
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -D__stMAMVIEW__ -c $< -o $@ $(INCLUDE)

sample1: $(OBJS)
	$(CC) $(CFLAGS) -D__stMAMVIEW__ $(OBJS) -o sample1 $(INCLUDE) $(LIBPATH) $(LIBS)

clean:
	-rm sample1 *.o
