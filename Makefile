
CC = gcc
INCLUDES ='-L/usr/X11R6/lib -L/usr/include'
LIBS='-lXaw -lXt -lX11'


all: build

build: flipse

flipse:
	$(CC) -o flipse flipse.c $(LIBS) $(INCLUDES)
