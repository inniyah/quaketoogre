OUTDIR = out
BIN = QuakeToOgre
OBJ = \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	Main.o \
	Quake.o \
	MD2Structure.o \
	MD3Structure.o \
	Animation.o \
	XmlWriter.o \
	Q2ModelToMesh.o \
	Q3ModelToMesh.o \
	md5mesh.o \
	md5anim.o \
	MD5ModelToMesh.o \
	MD5SkeletonBuilder.o

CC = g++
INCLUDE =
LIB = -lm

all : $(OBJ)
	$(CC) -o $(OUTDIR)/$(BIN) $(OBJ) $(LIB)

rebuild: clean all

clean:
	rm -rf $(OBJ)

%.o : %.cpp
	$(CC) -c $< $(INCLUDE) -o $@
