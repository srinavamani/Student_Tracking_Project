CC=gcc
CFLAGS=-D RPI
APPNAME = "BLE_Student_ID_Scanner"
#LP=-lpthread -lbluetooth -lxml2 -lsqlite3
LP=-lpthread -lxml2 -lsqlite3

all: scanner.o
		$(CC) $(CFLAGS) -o $(APPNAME) scanner.o -fno-stack-protector $(LP)
clean: 
		rm $(APPNAME) *.o
