CC = g++
CFLAGS = -g -Wall
SRCS = capture.cpp
PROG = capture

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
