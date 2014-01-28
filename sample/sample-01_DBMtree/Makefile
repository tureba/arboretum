# Makefile for GCC
#
# Author: Marcos Rodrigues Vieira
# Author: Fabio Jun Takada Chino
#
CXXFLAGS += -I../.. -g
LDFLAGS += -lstdc++ -lm -larboretum -L../..
SRC= app.cpp Cities.cpp city.cpp
OBJS=$(subst .cpp,.o,$(SRC))

all: Cities

Cities: $(OBJS)
#	-$(LD) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

#%.o: %.cpp $(HEADERS)

clean:
	-rm -f Cities *.o
