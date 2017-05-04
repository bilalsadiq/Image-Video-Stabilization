CC = g++
CFLAGS = -g -Wall
SRCS = BackgroundSubtraction.cpp
PROG = BackgroundSubtraction

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
