CC=gcc
CFLAGS=-D RPI
APPNAME_READER = "single_antenna"
LP1=-lpthread -lxml2 -lsqlite3

all: single_antenna.o
		$(CC) $(CFLAGS) -o $(APPNAME_READER) single_antenna.o -fno-stack-protector $(LP1)
clean: 
		rm $(APPNAME_READER) *.o
