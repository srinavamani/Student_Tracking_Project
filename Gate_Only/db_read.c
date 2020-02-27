#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sqlite3.h>
#include "read.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main(int argc, char *argv[])
{

// Coding starts from here,
system("rm /home/root/ID.db");

csv_access();

if(db_init() == 0)
{
	printf("ID.db Init Success\n");
csv_db();
printf("CSV converted into db file\n");
check_timing();
}
else
{
	printf("Run the binary as follows,\n");
	printf("./<binary name> <morning/afternoon> <number_of_channel> <IN> <OUT>\n");
}
}
}
