OUTDIR = out
BIN = QuakeToOgre
OBJ = \
	Main.o \
	Quake.o \
	MD2Structure.o \
	MD3Structure.o \
	Animation.o \
	XmlWriter.o \
	MD3XmlWriter.o \
	Q3ModelToMesh.o \
	Q3ModelToSkeleton.o

CC = g++
INCLUDE =
LIB = -lm

all : $(OBJ)
	$(CC) -o $(OUTDIR)/$(BIN) $(OBJ) $(LIB)

rebuild: clean all

clean:
	rm -rf $(OBJ)
	rm -rf $(OUTDIR)/*

%.o : %.cpp
	$(CC) -c $< $(INCLUDE) -o $@
