//
//	STUDENT TRACKING PROJECT
//
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
#include "scanner.h"

#define MORNING "morning"
#define AFTERNOON "afternoon"

pthread_t UHF_uart_reader_thread;
pthread_t timestamp_thread;

long int current_time = 0;
//char *timestamp = (char *)malloc(sizeof(char) * 16);
char timestamp[11];
char time_buff[5];
int section=0; // 1 - morning and 2 = afternoon
	
// This thread will receive tag ID via uart and update in sql - database.
void *UHF_uart_reader()
{
	int port_status = openport(MODEMDEVICE);
	int rdlen, byte_count=0;
	unsigned char hex[1], ID_data[21], ID_Number[30], ID_Bat_Value[3], ID_Rssi[3];
	time_t ltime;
	struct tm *tm;
	int i = 0, j = 0, packet = 0,bytes_to_read;

	if(port_status == 1)
	{
		printf("Port Available\n");
		bytes_to_read = 1;

// testing channel count
int sample_count=0, channel_number = 0;

	while(1)
	{
		rdlen = read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
		if(rdlen > 0)
		{
			if(ID_data[0] == 0xa0 && ID_data[1] == 0x13 && ID_data[20] != 0x00)
			{
				ID_data[byte_count] = '\0';
				j=0;
				for(i=7;i<19;i++)
				{
					ID_Number[j++] = MSBOF4BIT(ID_data[i]);
					ID_Number[j++] = LSBOF4BIT(ID_data[i]);
				}
				ID_Number[j] = '\0';				

				channel_number = channel_check(ID_data[4]);
//				printf("channel_number = %d\n",channel_number);
//				printf("channel = %02x\n",ID_data[4]);

// Testing channel count
/*
sample_count++;
if(sample_count >= 1 && sample_count <= 10)
	channel_number = 1;
else if(sample_count >= 11 && sample_count <= 20)
	channel_number = 2;
else if(sample_count >= 21 && sample_count <= 30)
	channel_number = 3;
else if(sample_count >= 31 && sample_count <= 40)
	channel_number = 4;
else if(sample_count >= 41 && sample_count <= 50)
	channel_number = 5;
else if(sample_count >= 51 && sample_count <= 60)
	channel_number = 6;
*/
				printf("ID_Number = %s\n\n",ID_Number);
				structure_update(ID_Number, time_buff, channel_number);
//				db_update(ID_Number, timestamp, "Channel_1");

				byte_count = 0;
				memset(ID_data, '\0',21);
				memset(ID_Number, '\0', 30);
			}
			else if(ID_data[0] == 0xa0)
			{
				read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
				if(ID_data[0] == 0x13)
				{
					bytes_to_read = 19;
					read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
					bytes_to_read = 21;
				}
				memset(ID_data, '\0',21);
				memset(ID_Number, '\0', 30);				
			}
		}
		else
			sleep(1);
	}
	}
	else
		printf("COM_PORT Open Fail\n");
}

// This thread will store the current timestamp for every sec in a global variable.
void *timestamp_generator()
{ 
	time_t ltime;
	struct tm *tm;
//	char *time_buff=(char *)malloc(sizeof(char) * 16);

	while(1)
	{
		ltime=time(NULL);
		tm=localtime(&ltime);
		sprintf(timestamp, "%02d%02d%02d%02d%02d%02d",tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900 - 2000, tm->tm_hour, tm->tm_min, tm->tm_sec);
		sprintf(time_buff, "%02d%02d%02d",tm->tm_hour, tm->tm_min, tm->tm_sec);
		sscanf(time_buff, "%ld", &current_time);
		sleep(1);
	}
	return NULL; 
}

int main(int argc, char *argv[])
{
	printf("argc = %d\n",argc);

printf("\nStudent Tracking Project\nGate_Only\nVersion - %s\n",Version);

#ifdef PC
system("rm -rf /home/embedded/channel.db");
#else
system("rm -rf /home/root/channel.db");
#endif

csv_access();

int state = db_init();

if(state == 0)
{

csv_to_main_db();

		if(pthread_create(&UHF_uart_reader_thread, NULL, UHF_uart_reader, NULL)) {
			printf("Error creating thread\n");
			return 1;
		}
		
		if(pthread_create(&timestamp_thread, NULL, timestamp_generator, NULL)) {
			printf("Error creating thread\n");
			return 1;
		}

		if(pthread_join(UHF_uart_reader_thread, NULL)) {
			fprintf(stderr, "Error joining uart_reader_thread\n");
			return 2;
		}
}
else{
printf("Database_create Error\n");
}
}
