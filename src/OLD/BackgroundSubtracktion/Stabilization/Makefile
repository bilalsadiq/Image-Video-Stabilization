CC = g++
CFLAGS = -g -Wall
SRCS = stabilize.cpp
PROG = stabilize

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
