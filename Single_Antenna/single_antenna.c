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
#include "single_antenna.h"

pthread_t UHF_uart_reader_thread;
pthread_t timestamp_thread;

long int current_time = 0;
//char *timestamp = (char *)malloc(sizeof(char) * 16);
char timestamp[11];
char time_buff[25];
	
void *UHF_uart_reader_single_channel()
{
	int port_status = openport(MODEMDEVICE);
	int rdlen, byte_count=0;
	unsigned char hex[1], ID_data[20], ID_Number[12];
	time_t ltime;
	struct tm *tm;
	int i = 0, j = 0, packet = 0,bytes_to_read;

	if(port_status == 1)
	{
		printf("Port Available\n");
		bytes_to_read = 1;

int sample_count=0;

	while(1)
	{
		rdlen = read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
		if(rdlen > 0)
		{
			if(ID_data[0] == 0x11 && ID_data[2] == 0xee)
			{
				ID_data[byte_count] = '\0';
				j=0;
				for(i=4;i<16;i++)
				{
					ID_Number[j++] = MSBOF4BIT(ID_data[i]);
					ID_Number[j++] = LSBOF4BIT(ID_data[i]);
				}
				ID_Number[j] = '\0';				

				printf("ID_Number = %s\n\n",ID_Number);
				structure_update(ID_Number, time_buff);

				byte_count = 0;
				memset(ID_data, '\0',18);
				memset(ID_Number, '\0', 12);
			}
			else if(ID_data[0] == 0x11)
			{
				read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
				if(ID_data[0] == 0x00)
				{
					read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
					if(ID_data[0] == 0xee)
					{
						bytes_to_read = 15;
						read(UARTDriverInfo.iModemFd, ID_data, bytes_to_read);
						bytes_to_read = 18;
					}
				}
				memset(ID_data, '\0',18);
				memset(ID_Number, '\0', 12);				
			}
		}
		else
			sleep(1);
	}
	}
	else
		printf("COM_PORT Open Fail\n");
}

void *timestamp_generator()
{ 
	time_t ltime;
	struct tm *tm;

	while(1)
	{
		ltime=time(NULL);
		tm=localtime(&ltime);
		sprintf(timestamp, "%02d%02d%02d%02d%02d%02d",tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900 - 2000, tm->tm_hour, tm->tm_min, tm->tm_sec);
		sprintf(time_buff, "%02d:%02d:%02d",tm->tm_hour, tm->tm_min, tm->tm_sec);
//		sscanf(time_buff, "%ld", &current_time);
		sleep(1);
	}
	return NULL; 
}

int main()
{
system("rm -rf /home/root/channel.db");

csv_access();

int state = db_init();

if(state == 0)
{

csv_to_main_db();

	if(pthread_create(&UHF_uart_reader_thread, NULL, UHF_uart_reader_single_channel, NULL)) {
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
else
{
printf("Database_create Error\n");
}
}
