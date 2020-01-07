#define CSV_PATH				"/home/root/ID.csv"
#define DB_PATH					"/home/root/ID.db"
#define CHANNEL_DB_PATH			"/home/root/channel.db"

int student_count=0;
int section=0;
char Null[1] = "0";
int number_of_channel = 0;
char entry_logic_IN[25];
char entry_logic_OUT[25];

struct Student_details  
{ 
    int id; 
    char Roll_No[10];
    char Name[20]; 
    char ID_Number[25];
    char Phone_Number_1[12];
    char Phone_Number_2[12];
    char IN_Time[15];
    char OUT_Time[15];
    char IN_SMS_sent[2];
    char OUT_SMS_sent[2];
}; 

struct Student_details Student[10000];

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
strcpy(Student[student_count].IN_SMS_sent,"0");
strcpy(Student[student_count].OUT_SMS_sent,"0");
strcpy(Student[student_count].IN_Time,"0");
strcpy(Student[student_count].OUT_Time,"0");
z=5;
student_count++;
}
}
}
i++;
}    

fclose (file_state);
//student_count--;
printf("Student Count = %d\n",student_count);
}

int check_db_IN(char *Roll_No,char *Name, char *ID_Number, char *IN_Time, char *IN_SMS_sent, char *state)
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

	if(state == "init")
	{
		sprintf(sql, "create table if not exists Student_Details(%s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT);", "Roll_No", "Name", "ID_Number", "IN_Time", "IN_SMS_sent");
	}
	else if(state == "write")
	{
		sprintf(sql, "INSERT INTO Student_Details VALUES('%s', '%s', '%s', '%s', '%s');", Roll_No, Name, ID_Number, IN_Time, IN_SMS_sent);
	}
	else if(state == "update")
	{
		sprintf(sql, "UPDATE Student_Details SET IN_Time = '%s', IN_SMS_sent = '%s' WHERE ID_Number = '%s';", IN_Time, IN_SMS_sent, ID_Number);
	}

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

int check_db_OUT(char *Roll_No,char *Name, char *ID_Number, char *OUT_Time, char *OUT_SMS_sent, char *state)
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

	if(state == "init")
	{
		sprintf(sql, "create table if not exists Student_Details(%s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT);", "Roll_No", "Name", "ID_Number", "OUT_Time", "OUT_SMS_sent");
	}
	else if(state == "write")
	{
		sprintf(sql, "INSERT INTO Student_Details VALUES('%s', '%s', '%s', '%s', '%s');", Roll_No, Name, ID_Number, OUT_Time, OUT_SMS_sent);
	}
	else if(state == "update")
	{
		sprintf(sql, "UPDATE Student_Details SET OUT_Time = '%s', OUT_SMS_sent = '%s' WHERE ID_Number = '%s';", OUT_Time, OUT_SMS_sent, ID_Number);
	}

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

int csv_db()
{
int i = 0;
if(section==1)
{
for(i=0;i<student_count;i++)
check_db_IN(Student[i].Roll_No,Student[i].Name,Student[i].ID_Number,Student[i].IN_Time,Student[i].IN_SMS_sent,"write");
}
else if(section==2)
{
for(i=0;i<student_count;i++)
check_db_OUT(Student[i].Roll_No,Student[i].Name,Student[i].ID_Number,Student[i].OUT_Time,Student[i].OUT_SMS_sent,"write");
}
}

int db_init()
{
	if(section == 1)
		return check_db_IN("Roll_No", "Name", "ID_Number", "IN_Time", "SMS_status", "init");
	else if(section == 2)
		return check_db_OUT("Roll_No", "Name", "ID_Number", "OUT_Time", "SMS_status", "init");
}

int presence(char *ID_Number)
{
    int channel_1, channel_2, channel_3, channel_4, channel_5, channel_6, channel_7, channel_8;
    sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
	char sql[1000];
    
    int rc = sqlite3_open(CHANNEL_DB_PATH, &db);

	printf("ID_Number = %s\n",ID_Number);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }

	sprintf(sql,"SELECT Channel_1, Channel_2, Channel_3, Channel_4, Channel_5, Channel_6, Channel_7, Channel_8 FROM Student_Details WHERE ID_Number='%s';",ID_Number);
	
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    if (rc == SQLITE_OK) {
        
        sqlite3_bind_int(res, 1, 3);
    } else {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

	int step = sqlite3_step(res);

	if (step == SQLITE_ROW)
	{
		sscanf(sqlite3_column_text(res, 0), "%d", &channel_1);
		sscanf(sqlite3_column_text(res, 1), "%d", &channel_2);
		sscanf(sqlite3_column_text(res, 2), "%d", &channel_3);
		sscanf(sqlite3_column_text(res, 3), "%d", &channel_4);
		sscanf(sqlite3_column_text(res, 4), "%d", &channel_5);
		sscanf(sqlite3_column_text(res, 5), "%d", &channel_6);
		sscanf(sqlite3_column_text(res, 6), "%d", &channel_7);
		sscanf(sqlite3_column_text(res, 7), "%d", &channel_8);
//		printf("channel1 - %d\n",channel_1);
//		printf("channel2 - %d\n",channel_2);
	}
	sqlite3_finalize(res);
    sqlite3_close(db);

int i=0;
int IN_time=0, IN_time_main=0, IN_time_sub=0, IN_time_back=0;
int OUT_time=0, OUT_time_main=0, OUT_time_sub=0, OUT_time_back=0;

// IN & OUT Logic
if(section == 1)
{
	for(i=0;i<sizeof(entry_logic_IN);i++)
	{
	if(entry_logic_IN[i] == '_')
	{
		IN_time_main = IN_time;
		IN_time = 0;
//		printf("Receiver _ main - %d\n",IN_time_main);
	}
	else if(entry_logic_IN[i] == '\0')
	{
		if(IN_time_main == 0)
		{
			IN_time_main = IN_time;
			IN_time = 0;
//			printf("Receiver Null - main - %d\n",IN_time_main);
		}
		else
		{
			IN_time_sub = IN_time;
			IN_time = 0;
//			printf("Receiver Null - sub - %d\n",IN_time_sub);
		}
		break;
	}
	else if(entry_logic_IN[i] != '_')
	{
//		printf("channel_number - %c\n",entry_logic_IN[i]);

		if(entry_logic_IN[i] == '1')
			IN_time_back = channel_1;
		if(entry_logic_IN[i] == '2')
			IN_time_back = channel_2;
		if(entry_logic_IN[i] == '3')
			IN_time_back = channel_3;
		if(entry_logic_IN[i] == '4')
			IN_time_back = channel_4;
		if(entry_logic_IN[i] == '5')
			IN_time_back = channel_5;
		if(entry_logic_IN[i] == '6')
			IN_time_back = channel_6;
		if(entry_logic_IN[i] == '7')
			IN_time_back = channel_7;
		if(entry_logic_IN[i] == '8')
			IN_time_back = channel_8;

		if(IN_time <= IN_time_back)
		{
			IN_time = IN_time_back;
//			printf("IN_time = %d\n",IN_time);
		}
	}
	}
/*	
	if(IN_time_sub > IN_time_main)
	{
//		printf("===========> IN_time = %d\n",IN_time_sub);
		return IN_time_sub;
	}
	else if(IN_time_sub == 0 && IN_time_main != 0)
	{
		if(number_of_channel == 1)
		{
//			printf("===========> IN_time = %d\n",IN_time_main);
			return IN_time_main;
		}
		else
		{
			IN_time_main = 0;
//			printf("===========> IN_time = %d\n",IN_time_main);
			return IN_time_main;
		}
	}
	else
	{
		return 0;
	}
*/

if(IN_time_main > IN_time_sub)
	return IN_time_main;
else if(IN_time_sub > IN_time_main)
	return IN_time_sub;

}
else if(section == 2)
{
	for(i=0;i<sizeof(entry_logic_OUT);i++)
	{
	if(entry_logic_OUT[i] == '_')
	{
		OUT_time_main = OUT_time;
		OUT_time = 0;
//		printf("Receiver _ main - %d\n",OUT_time_main);
	}
	else if(entry_logic_OUT[i] == '\0')
	{
		if(OUT_time_main == 0)
		{
			OUT_time_main = OUT_time;
			OUT_time = 0;
//			printf("Receiver Null - main - %d\n",OUT_time_main);
		}
		else
		{
			OUT_time_sub = OUT_time;
			OUT_time = 0;
//			printf("Receiver Null - sub - %d\n",OUT_time_sub);
		}
		break;
	}
	else if(entry_logic_OUT[i] != '_')
	{
//		printf("channel_number - %c\n",entry_logic_OUT[i]);

		if(entry_logic_OUT[i] == '1')
			OUT_time_back = channel_1;
		if(entry_logic_OUT[i] == '2')
			OUT_time_back = channel_2;
		if(entry_logic_OUT[i] == '3')
			OUT_time_back = channel_3;
		if(entry_logic_OUT[i] == '4')
			OUT_time_back = channel_4;
		if(entry_logic_OUT[i] == '5')
			OUT_time_back = channel_5;
		if(entry_logic_OUT[i] == '6')
			OUT_time_back = channel_6;
		if(entry_logic_OUT[i] == '7')
			OUT_time_back = channel_7;
		if(entry_logic_OUT[i] == '8')
			OUT_time_back = channel_8;

		if(OUT_time <= OUT_time_back)
		{
			OUT_time = OUT_time_back;
//			printf("OUT_time = %d\n",OUT_time);
		}
		else
		{
//			printf("lesser value - omitted\n");
		}
	}
	}
/*	
	if(OUT_time_sub > OUT_time_main)
	{
//		printf("===========> OUT_time = %d\n",OUT_time_sub);
		return OUT_time_sub;
	}
	else if(OUT_time_sub == 0 && OUT_time_main != 0)
	{
		if(number_of_channel == 1)
		{
//			printf("===========> OUT_time = %d\n",OUT_time_main);
			return OUT_time_main;
		}
		else
		{
			OUT_time_main = 0;
//			printf("===========> OUT_time = %d\n",OUT_time_main);
			return OUT_time_main;
		}
	}
	else
	{
		return 0;
	}
*/

if(OUT_time_main > OUT_time_sub)
	return OUT_time_main;
else if(OUT_time_sub > OUT_time_main)
	return OUT_time_sub;

}
}

int check_timing()
{
int i = 0;
int time_value=0;
char entry_time[10];

for(i=0;i<=student_count-1;i++)
{
time_value = presence(Student[i].ID_Number);

if(time_value == 0)
	strcpy(entry_time,Null);
else
	sprintf(entry_time,"%d",time_value);
	
printf("time = %s\n",entry_time);


	if(section == 1)
	{
		strcpy(Student[i].IN_Time,entry_time);
		check_db_IN(Student[i].Roll_No, Student[i].Name, Student[i].ID_Number, Student[i].IN_Time, Student[i].IN_SMS_sent,"update");
	}
	else
	{
		strcpy(Student[i].OUT_Time,entry_time);
		check_db_OUT(Student[i].Roll_No, Student[i].Name, Student[i].ID_Number, Student[i].OUT_Time, Student[i].OUT_SMS_sent,"update");
	}
}
}
