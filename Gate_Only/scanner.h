#include <stdbool.h>

#ifdef PC
#define DB_PATH					"/home/embedded/ID.db"
#define CSV_PATH				"/home/embedded/ID.csv"
#else
#define DB_PATH					"/home/root/channel.db"
#define CSV_PATH				"/home/root/ID.csv"
#endif

#define Version					"1.0"
#define MODEMDEVICE				"/dev/ttyUSB0"

int callback(void *NotUsed, int argc, char **argv, char **azColName);
int db_update(char *ID_Number, char *IN_Time, char *OUT_Time);

typedef struct {
	int iModemFd;
	struct termios options;
	speed_t baudrate ;
	pthread_mutex_t UARTLock;
} UARTDriverInfo_t;

struct Student_details  
{ 
    int id; 
    char Roll_No[10];
    char Name[20]; 
    char ID_Number[25];
    char Phone_Number_1[12];
    char Phone_Number_2[12];
    char IN_Time[10];
    char OUT_Time[10];
}; 

struct Student_details Student[10000];
UARTDriverInfo_t UARTDriverInfo;

int student_count=0;

int csv_access()
{
int i=0,j=0,z=0;
char data[100];
FILE *file_state;
char buffer[350000]; 
   
file_state = fopen (CSV_PATH, "r"); 
if (file_state == NULL) 
{ 
fprintf(stderr, "\nError opening file\n"); 
exit (1); 
} 
fread(buffer,350000,1,file_state);

while(buffer[i] != '\0')
{
if(buffer[i] != ',' && buffer[i] != '\n')
data[j++] = buffer[i];
else
{
data[j]='\0';
j=0;
z++;

if(z>=5)
{
if(z==6)
strcpy(Student[student_count].Roll_No,data);
else if(z==7)
strcpy(Student[student_count].Name,data);
else if(z==8)
strcpy(Student[student_count].ID_Number,data);
else if(z==9)
strcpy(Student[student_count].Phone_Number_1,data);
else if(z==10)
{
strcpy(Student[student_count].Phone_Number_2,data);
strcpy(Student[student_count].IN_Time,"\0");
strcpy(Student[student_count].OUT_Time,"\0");
z=5;
student_count++;
}
}
}
i++;
}    
fclose (file_state);
printf("Student Count = %d\n",student_count);
}

int structure_update(char ID_Number[25], char timestamp[25], int channel_number)
{
int i = 0;
char channel[8];
	for(i=0;i<=student_count;i++)
	{
		if(strcmp(Student[i].ID_Number,ID_Number) == 0)
		{
			if(channel_number == 1 && strcmp(Student[i].IN_Time,"\0") == 0)
			{
				strcpy(Student[i].IN_Time,timestamp);
//				printf("IN_Time - %s	OUT_Time - %s\n",Student[i].IN_Time, Student[i].OUT_Time);
				db_update(ID_Number, Student[i].IN_Time, Student[i].OUT_Time);
//				printf("ID - %s		timestamp - %s		channel - %d\n",ID_Number, timestamp, channel_number);
				system("echo 1 > /sys/class/gpio/gpio21/value");
				sleep(1);
				system("echo 0 > /sys/class/gpio/gpio21/value");
			}
			else if(channel_number == 2 && strcmp(Student[i].OUT_Time,"\0") == 0)
			{
				strcpy(Student[i].OUT_Time,timestamp);
//				printf("IN_Time - %s	OUT_Time - %s\n",Student[i].IN_Time, Student[i].OUT_Time);
				db_update(ID_Number, Student[i].IN_Time, Student[i].OUT_Time);
//				printf("ID - %s		timestamp - %s		channel - %d\n",ID_Number, timestamp, channel_number);
				system("echo 1 > /sys/class/gpio/gpio21/value");
				sleep(1);
				system("echo 0 > /sys/class/gpio/gpio21/value");
			}
		}
	}
}

int LSBOF4BIT(unsigned char hex)
{
char value;
unsigned char hex_data = hex & 0x0f;

if(hex_data >= 0x00 && hex_data <= 0x09)
        value = hex_data+48;
else if(hex_data >= 0x0a && hex_data <= 0x0f)
        value = hex_data+97-10;
else if(hex_data >= 0x0A && hex_data <= 0x0F)
        value = hex_data+65-10;

return value;
}

int MSBOF4BIT(unsigned char hex)
{
char value;
unsigned char hex_data = (hex >> 4) & 0x0f;

if(hex_data >= 0x00 && hex_data <= 0x09)
        value = hex_data+48;
else if(hex_data >= 0x0a && hex_data <= 0x0f)
        value = hex_data+97-10;
else if(hex_data >= 0x0A && hex_data <= 0x0F)
        value = hex_data+65-10;

return value;
}

int openport(char *dev_name)
{
	UARTDriverInfo.iModemFd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (UARTDriverInfo.iModemFd < 0) {
		printf("%s\n", "Open Error");
		return 0;
	}
	fcntl(UARTDriverInfo.iModemFd, F_SETFL, O_RDWR);
	tcgetattr(UARTDriverInfo.iModemFd, &UARTDriverInfo.options);
	UARTDriverInfo.options.c_cflag |= (CLOCAL | CREAD);
	UARTDriverInfo.options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	UARTDriverInfo.options.c_oflag &= ~OPOST;
	cfmakeraw(&UARTDriverInfo.options);
	UARTDriverInfo.options.c_cc[VMIN] = 1;
	UARTDriverInfo.options.c_cc[VTIME] = 0;
	UARTDriverInfo.baudrate = B115200;
	cfsetospeed(&UARTDriverInfo.options, UARTDriverInfo.baudrate);
	cfsetispeed(&UARTDriverInfo.options, UARTDriverInfo.baudrate);
	tcsetattr(UARTDriverInfo.iModemFd, TCSAFLUSH, &UARTDriverInfo.options);
	sleep(2);
	tcflush(UARTDriverInfo.iModemFd, TCIOFLUSH);
	return 1;
}

int db_init()
{
	sqlite3 *db;
	char *err_msg = 0;
	sqlite3_stmt *res;

	int rc = sqlite3_open(DB_PATH, &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	char sql[1000];

	sprintf(sql, "create table if not exists Student_Details(%s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT);", "Roll_No", "Name", "ID_Number", "IN_Time", "OUT_Time");

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK ) {

		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);        
		sqlite3_close(db);

		return 1;
	} 

	sqlite3_close(db);

	return 0;
}

int db_write(char *Roll_No, char *Name, char *ID_Number, char *IN_Time, char *OUT_Time)
{
	sqlite3 *db;
	char *err_msg = 0;
	sqlite3_stmt *res;

	int rc = sqlite3_open(DB_PATH, &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	char sql[1000];

	sprintf(sql, "INSERT INTO Student_Details('Roll_No','Name','ID_Number','IN_Time','OUT_Time') VALUES('%s','%s','%s','%s','%s');", Roll_No, Name, ID_Number, IN_Time, OUT_Time);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK ) {

		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);        
		sqlite3_close(db);

		return 1;
	} 

	sqlite3_close(db);

	return 0;
}

int csv_to_main_db()
{
int i = 0;
for(i=0;i<student_count;i++)
db_write(Student[i].Roll_No, Student[i].Name, Student[i].ID_Number, Student[i].IN_Time, Student[i].OUT_Time);
}

int db_update(char *ID_Number, char *IN_Time, char *OUT_Time)
{
	printf("ID - %s		IN - %s		OUT - %s\n",ID_Number, IN_Time, OUT_Time);
	sqlite3 *db;
	char *err_msg = 0;
	sqlite3_stmt *res;

	int rc = sqlite3_open(DB_PATH, &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	char sql[1000];

	sprintf(sql, "UPDATE Student_Details set IN_Time = '%s', OUT_Time = '%s' where ID_Number='%s';", IN_Time, OUT_Time, ID_Number);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK ) {

		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);        
		sqlite3_close(db);

		return 1;
	} 

	sqlite3_close(db);

	return 0;
}


int channel_check(char channel_value)
{
int i=0, channel_count=0;
bool bit[8];

for(i = 0; i < 8; i++) {
     bit[i] = ((channel_value >> i) & 0x01);
}

if(bit[5] == 0)
{
if(bit[1] == 0 && bit[0] == 0)
	channel_count=1;
else if(bit[1] == 0 && bit[0] == 1)
	channel_count=2;
if(bit[1] == 1 && bit[0] == 0)
	channel_count=3;
if(bit[1] == 1 && bit[0] == 1)
	channel_count=4;	
}
else if(bit[5] == 1)
{
if(bit[1] == 0 && bit[0] == 0)
	channel_count=5;
else if(bit[1] == 0 && bit[0] == 1)
	channel_count=6;
if(bit[1] == 1 && bit[0] == 0)
	channel_count=7;
if(bit[1] == 1 && bit[0] == 1)
	channel_count=8;			
}

//printf("channel_count = %d\n",channel_count);

return channel_count;
}
