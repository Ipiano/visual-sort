# Makefile for CSC300 PA#2 (quadtree program) using OpenGL and GLUT.

# Author: Andrew Stelter, Jessi Thompson
# Class:  CSC300 Data Structures
# Date:   Fall 2015

# Usage:  make target1 target2 ...

#-----------------------------------------------------------------------

# GNU C/C++ compiler and linker:
LINK = g++

# Turn on optimization and warnings, use c++11:
CFLAGS = -std=c++11 -Wall -O -fpermissive
CXXFLAGS = $(CFLAGS)

# OpenGL/Mesa libraries for Linux
GL_LIBS = -lglut -lGLU -lGL -lm
THREAD_LIBS = -lpthread

#-----------------------------------------------------------------------
# Specific targets:

# MAKE allows the use of "wildcards", to make writing compilation instructions
# a bit easier. GNU make uses $@ for the target and $^ for the dependencies.

all:	visusort

visusort:	Main.o Callbacks.o Globals.o SortHandler.o Sorts.o
	$(LINK) -o $@ $^ $(GL_LIBS) $(THREAD_LIBS)
	
debug:	CXXFLAGS += -g
debug:  visusort

clean:
	rm -f *.o *~ core a.out

remake: clean all
