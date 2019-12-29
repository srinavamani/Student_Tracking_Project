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
int db_update(char *ID_Number, char *timestamp, char *channel_number);

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
    char channel_1[5];
    char channel_2[5];
    char channel_3[5];
    char channel_4[5];
    char channel_5[5];
    char channel_6[5];    
    char channel_7[5];
    char channel_8[5];
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
strcpy(Student[student_count].channel_1,"0");
strcpy(Student[student_count].channel_2,"0");
strcpy(Student[student_count].channel_3,"0");
strcpy(Student[student_count].channel_4,"0");
strcpy(Student[student_count].channel_5,"0");
strcpy(Student[student_count].channel_6,"0");
strcpy(Student[student_count].channel_7,"0");
strcpy(Student[student_count].channel_8,"0");
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
if(channel_number == 1 && strcmp(Student[i].channel_1,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_1");
	strcpy(Student[i].channel_1,timestamp);
}
else if(channel_number == 2 && strcmp(Student[i].channel_2,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_2");
	strcpy(Student[i].channel_2,timestamp);
}
else if(channel_number == 3 && strcmp(Student[i].channel_3,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_3");
	strcpy(Student[i].channel_3,timestamp);
}
else if(channel_number == 4 && strcmp(Student[i].channel_4,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_4");
	strcpy(Student[i].channel_4,timestamp);
}
else if(channel_number == 5 && strcmp(Student[i].channel_5,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_5");
	strcpy(Student[i].channel_5,timestamp);
}
else if(channel_number == 6 && strcmp(Student[i].channel_6,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_6");
	strcpy(Student[i].channel_6,timestamp);
}
else if(channel_number == 7 && strcmp(Student[i].channel_7,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_7");
	strcpy(Student[i].channel_7,timestamp);
}
else if(channel_number == 8 && strcmp(Student[i].channel_8,"0") == 0)
{
	db_update(ID_Number, timestamp, "Channel_8");
	strcpy(Student[i].channel_8,timestamp);
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

	sprintf(sql, "create table if not exists Student_Details(%s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT);", "ID_Number", "Channel_1", "Channel_2", "Channel_3", "Channel_4", "Channel_5", "Channel_6", "Channel_7", "Channel_8");

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

int db_write(char *ID_Number, char *channel_1, char *channel_2, char *channel_3, char *channel_4, char *channel_5, char *channel_6, char *channel_7, char *channel_8)
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

	sprintf(sql, "INSERT INTO Student_Details('ID_Number','Channel_1','Channel_2','Channel_3','Channel_4','Channel_5','Channel_6','Channel_7','Channel_8') VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s');", ID_Number, channel_1, channel_2, channel_3, channel_4, channel_5, channel_6, channel_7, channel_8);

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
db_write(Student[i].ID_Number, Student[i].channel_1, Student[i].channel_2, Student[i].channel_3, Student[i].channel_4, Student[i].channel_5, Student[i].channel_6, Student[i].channel_7, Student[i].channel_8);
}

int db_update(char *ID_Number, char *timestamp, char *channel)
{
	printf("channel = %s		%s\n",channel, ID_Number);
	sqlite3 *db;
	char *err_msg = 0;
	sqlite3_stmt *res;

	int rc = sqlite3_open(DB_PATH, &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	char sql[1000];

	sprintf(sql, "UPDATE Student_Details set '%s' = '%s' where ID_Number='%s';", channel, timestamp, ID_Number);

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
