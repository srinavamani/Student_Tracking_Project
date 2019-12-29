#define CSV_PATH				"/home/root/ID.csv"
#define DB_PATH					"/home/root/ID.db"
#define CHANNEL_DB_PATH			"/home/root/channel.db"

int student_count=0;
int section=0;
char Null[1] = "0";

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

int check_db(char *Roll_No,char *Name, char *ID_Number, char *IN_Time, char *OUT_Time, char *IN_SMS_sent, char *OUT_SMS_sent, char *state)
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
		sprintf(sql, "create table if not exists Student_Details(%s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT);", "Roll_No", "Name", "ID_Number", "IN_Time", "OUT_Time", "IN_SMS_sent", "OUT_SMS_sent");
	}
	else if(state == "write")
	{
		sprintf(sql, "INSERT INTO Student_Details VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s');", Roll_No, Name, ID_Number, IN_Time, OUT_Time, IN_SMS_sent, OUT_SMS_sent);
	}
	else if(state == "update")
	{
		sprintf(sql, "UPDATE Student_Details SET IN_Time = '%s', OUT_Time = '%s', IN_SMS_sent = '%s', OUT_SMS_sent = '%s' WHERE ID_Number = '%s';", IN_Time, OUT_Time, IN_SMS_sent, OUT_SMS_sent, ID_Number);
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
for(i=0;i<student_count;i++)
check_db(Student[i].Roll_No,Student[i].Name,Student[i].ID_Number,Student[i].IN_Time,Student[i].OUT_Time,Student[i].IN_SMS_sent,Student[i].OUT_SMS_sent,"write");
}

int db_init()
{
	return check_db("Roll_No", "Name", "ID_Number", "IN_Time", "OUT_Time", "IN_SMS_sent", "OUT_SMS_sent","init");
}

int presence(char *ID_Number)
{
    int channel_1, channel_2;
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

	sprintf(sql,"SELECT Channel_1, Channel_2 FROM Student_Details WHERE ID_Number='%s';",ID_Number);
	
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
//		printf("channel1 - %d\n",channel_1);
//		printf("channel2 - %d\n",channel_2);
	}
	sqlite3_finalize(res);
    sqlite3_close(db);

	if(channel_1 != 0 && channel_2 != 0)
	{
		if(channel_1 >= channel_2)
		{
//			printf("time_channel_1 = %d\n",channel_1);
			return channel_1;
		}
		if(channel_2 >= channel_1)
		{
//			printf("time_channel_2 = %d\n",channel_2);
			return channel_2;
		}
	}

	return 0;
}

int check_timing()
{
int i = 0;
int time_value=0;
char entry_time[10];

for(i=0;i<=student_count-1;i++)
{
time_value = presence(Student[i].ID_Number);
printf("time_value_int = %d\n",time_value);

if(time_value == 0)
	strcpy(entry_time,Null);
else
	sprintf(entry_time,"%d",time_value);
	
printf("time = %s\n",entry_time);


	if(section == 1)
	{
		strcpy(Student[i].IN_Time,entry_time);
	}
	else
	{
		strcpy(Student[i].OUT_Time,entry_time);
	}
check_db(Student[i].Roll_No, Student[i].Name, Student[i].ID_Number, Student[i].IN_Time, Student[i].OUT_Time, Student[i].IN_SMS_sent,Student[i].OUT_SMS_sent,"update");
}
}
