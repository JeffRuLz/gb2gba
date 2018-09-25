CC = g++

INCLUDE_PATHS =
LIBRARY_PATHS =

CFLAGS = -D_WIN32
LIBS = -lmingw32
TARGET = ../gb2gba

OBJS  = $(wildcard src/*.c)

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CFLAGS) $(LIBS) -o $(TARGET)