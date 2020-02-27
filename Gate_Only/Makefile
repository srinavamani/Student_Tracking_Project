CC=gcc
CFLAGS=-D RPI
APPNAME_READER = "BLE_Student_ID_Scanner"
#APPNAME_UPLOAD = "Upload"
#LP=-lpthread -lbluetooth -lxml2 -lsqlite3
LP1=-lpthread -lxml2 -lsqlite3
LP2=-lsqlite3

#all: scanner.o db_read.o
all: scanner.o
		$(CC) $(CFLAGS) -o $(APPNAME_READER) scanner.o -fno-stack-protector $(LP1)
#		$(CC) $(CFLAGS) -o $(APPNAME_UPLOAD) db_read.o -fno-stack-protector $(LP2)
clean: 
#		rm $(APPNAME_READER) $(APPNAME_UPLOAD) *.o
		rm $(APPNAME_READER) *.o
