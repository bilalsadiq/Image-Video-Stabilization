CC = g++
CFLAGS = -g -Wall
SRCS = project1c.cpp
PROG = project1c

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
