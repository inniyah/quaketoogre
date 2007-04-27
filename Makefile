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
	Q3ModelToMesh.o

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
