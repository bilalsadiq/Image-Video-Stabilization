CC = g++
CFLAGS = -g -Wall
SRCS = gmg.cpp
PROG = BackgroundSubtractionGMG

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
