CC = g++
CFLAGS = -g -Wall
SRCS = VideoStabilization.cpp
PROG = videoStabilization2

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
