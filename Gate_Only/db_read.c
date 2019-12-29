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

#define MORNING "morning"
#define AFTERNOON "afternoon"

int callback(void *NotUsed, int argc, char **argv, char **azColName);

int state = 0;
int number_of_channel = 0;
char entry_logic_IN[25];
char entry_logic_OUT[25];

int main(int argc, char *argv[])
{
printf("argc = %d\n",argc);

if(argc==5)
{
if(strcmp(argv[1],MORNING)==0)
section=1;
else if(strcmp(argv[1],AFTERNOON)==0)
section=2;

printf("section - %d\n",section);

sscanf(argv[2], "%d", &number_of_channel);
printf("Number of channel = %d\n",number_of_channel);
state = number_of_channel;
//IN Logic
strcpy(entry_logic_IN,argv[3]);
printf("IN Logic = %s\n",entry_logic_IN);

//OUT Logic
strcpy(entry_logic_OUT,argv[4]);
printf("OUT Logic = %s\n",entry_logic_OUT);


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
