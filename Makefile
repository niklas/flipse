
CC = gcc
CCOPTS=-Wall
INCLUDES =-L/usr/X11R6/lib -L/usr/include
LIBS=-lXaw -lXt -lX11


all: build

build: flipse

flipse: flipse.c
	$(CC) -o flipse flipse.c $(CCOPTS) $(LIBS) $(INCLUDES)


clean:
	rm flipse
