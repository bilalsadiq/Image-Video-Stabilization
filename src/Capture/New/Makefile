CC = g++
CFLAGS = -g -Wall
SRCS = BodyDetection.cpp
PROG = bodyDetection

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
