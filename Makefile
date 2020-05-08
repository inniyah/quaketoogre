#!/usr/bin/make -f

CC= gcc
CXX= g++
RM= rm -f

PKGCONFIG= pkg-config
PACKAGES= tinyxml

CFLAGS= \
	-Wall \
	-fstack-protector-strong \
	-Wall \
	-Wformat \
	-Werror=format-security \
	-Wdate-time \
	-D_FORTIFY_SOURCE=2 \
	$(shell $(PKGCONFIG) --cflags $(PACKAGES))

LDFLAGS= \
	-Wl,--as-needed \
	-Wl,--no-undefined \
	-Wl,--no-allow-shlib-undefined

CSTD=-std=c11
CPPSTD=-std=c++11

OPTS= -O2 -g
DEFS=
INCS=

LIBS= $(shell $(PKGCONFIG) --libs $(PACKAGES)) -lm

BINARY= QuakeToOgre

BINARY_SRCS= \
	Main.cpp \
	Quake.cpp \
	MD2Model.cpp \
	MD3Model.cpp \
	Animation.cpp \
	XmlWriter.cpp \
	Q2ModelToMesh.cpp \
	Q3ModelToMesh.cpp \
	md5mesh.cpp \
	md5anim.cpp \
	MD5ModelToMesh.cpp \
	quaternion.cpp \
	vector.cpp \
	StringUtil.cpp

BINARY_OBJS= $(subst .cpp,.o,$(BINARY_SRCS))

all: $(BINARY)

$(BINARY): $(BINARY_OBJS)
	$(CXX) $(CPPSTD) $(CSTD) $(LDFLAGS) -o $@ $(BINARY_OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

%.o: %.cc
	$(CXX) $(CPPSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

%.o: %.c
	$(CC) $(CSTD) $(OPTS) -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

depend: .depend

.depend: $(PINOCCHIO_SRCS) $(BINARY_SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPSTD) $(DEFS) $(INCS) $(CFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(PINOCCHIO_OBJS) $(BINARY_OBJS) $(BINARY)
	$(RM) -fv *~ .depend core *.out *.bak
	$(RM) -fv *.o *.a *~
	$(RM) -fv */*.o */*.a */*~

include .depend

.PHONY: all depend clean
