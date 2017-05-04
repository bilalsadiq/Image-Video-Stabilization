CC = g++
CFLAGS = -g -Wall
SRCS = objectTracking.cpp
PROG = objectTracking

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
