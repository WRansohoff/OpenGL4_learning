SRC = util.cpp math2d.cpp math3d.cpp main.cpp
CC = g++
CFLAGS = -std=c++11
LFLAGS = -lGL -lGLU -lGLEW -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor -lm -ldl

OUTPUT = main

all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LFLAGS) -o $(OUTPUT)
