# Makefile for GCC
#
# Author: Fabio Jun Takada Chino
# Author: Marcos Rodrigues Vieira
#
CC=gcc
AR=ar
CFLAGS=
prefix?=/usr/local
exec-prefix?=/usr/local
SRCPATH=../../src
INCLUDEPATH=../../include
INCLUDE=-I$(INCLUDEPATH)
SRC=	$(SRCPATH)/CStorage.cpp \
	$(SRCPATH)/stTreeInformation.cpp \
	$(SRCPATH)/stDummyNode.cpp \
	$(SRCPATH)/stSlimNode.cpp \
	$(SRCPATH)/stDBMNode.cpp \
	$(SRCPATH)/stDFNode.cpp \
	$(SRCPATH)/stMNode.cpp \
	$(SRCPATH)/stVPNode.cpp \
	$(SRCPATH)/stGHNode.cpp \
	$(SRCPATH)/stUtil.cpp \
	$(SRCPATH)/stListPriorityQueue.cpp \
	$(SRCPATH)/stPage.cpp \
	$(SRCPATH)/stMemoryPageManager.cpp \
	$(SRCPATH)/stDiskPageManager.cpp \
	$(SRCPATH)/stPlainDiskPageManager.cpp
OBJS=$(subst .cpp,.o,$(SRC))
LIBNAME=../libarboretum.a

# Implicit Rules
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

# Rules
$(LIBNAME): $(OBJS)
	$(AR) -r $(LIBNAME) $(OBJS)

default: $(LIBNAME)

help:
	@echo Arboretum gcc Makefile
	@echo '
	@echo Targets:
	@echo    default: Build libarboretum.a
	@echo    help:    Prints this help screen
	@echo    clean:   Remove all .o files
	@echo    install: Install library and headers
	@echo '
	@echo The install target has two options:
	@echo    prefix = include prefix
	@echo    exec-prefix = lib prefix
	@echo '
	@echo e.g.: make install prefix=... exec-prefix=...
	@echo '
	@echo Current values are:
	@echo    prefix = $(prefix) [$(prefix)/include]
	@echo    exec-prefix = $(exec-prefix) [$(exec-prefix)/lib]

clean:
	rm -f $(SRCPATH)/*.o

install:
	@echo This target is not complete yet.
