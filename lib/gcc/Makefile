# Makefile for GCC
#
# Author: Fabio Jun Takada Chino
# Author: Marcos Rodrigues Vieira
# Revisor: Arthur Nascimento <tureba@gmail.com>
#
CXXFLAGS+=-pipe -fPIC
prefix?=/usr
lib_dest?=$(prefix)/lib
include_dest?=$(prefix)/include
doc_dest?=$(prefix)/share/doc/arboretum
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
LIBNAME=../libarboretum.so
build_doc?=yes

all: $(LIBNAME)

# Implicit Rules
%.o: %.cpp $(HEADERS)
	-$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# Rules
$(LIBNAME): $(OBJS)
	-$(LD) -shared -o $(LIBNAME) $(OBJS)

help:
	@echo "
	Arboretum Linux Makefile

	Targets:
		all/default: Build libarboretum
		help:        Print this help screen
		clean:       Remove all compilation files
		install:     Install library and headers

	The install target has one installation option: prefix.
	This option defines where to install arboretum (default: /usr).
	All paths are relative to prefix.

	e.g.: make install prefix=<someotherprefix>

	Current values are:
		prefix = $(prefix)
		library = $(lib_dest)
		includes = $(include_dest)
		documentation = $(doc_dest)"

doc:
ifeq ($(build_doc),yes)
	cd ../../docs && doxygen
endif

clean:
	-rm -f $(OBJS)

distclean: clean
	-rm -f $(LIBNAME)

ifeq ($(build_doc),yes)
install_doc: doc
	-find ../../docs -type d | sed -e 's=../../docs==' | xargs -n 1 -I '{}' install -m 755 -d "$(doc_dest)/{}"
	-find ../../docs -type f '!' \( -name Doxyfile -o -name doxygen.css -o -name doxygen.log -o -name footer.html -o -name header.html -o -name readme.txt \) | sed -e 's=../../docs==' | xargs -n 1 -I '{}' install -m 644 "../../docs/{}" "$(doc_dest)/{}"
else
install_doc:
	-[ -d "$(doc_dest)" ] || install -m 755 -d "$(doc_dest)"
	-install -m 644 ../../docs/license.txt "$(doc_dest)"
endif

install: all install_doc
	-[ -d $(prefix) ] || install -m 755 -d $(prefix)
	-[ -d $(lib_dest) ] || install -m 755 -d $(lib_dest)
	-install -m 755 $(LIBNAME) $(lib_dest)
	-find ../../include -type d | sed -e 's=../../include==' | xargs -n 1 -I '{}' install -m 755 -d "$(include_dest)/{}"
	-find ../../include -type f \( -name *.h -o -name *.cc -o -name *.cpp \) | sed -e 's=../../include==' | xargs -n 1 -I '{}' install -m 644 "../../include/{}" "$(include_dest)/{}"
