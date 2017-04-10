CC = g++
CFLAGS = -g -Wall
SRCS = Object_Tracking_and_Filtering.cpp
PROG = project1c

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
