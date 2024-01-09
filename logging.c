#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include "fparam.h"


void get_timestamp(char str[], size_t size)
{
	time_t rawtime;
	rawtime = time(NULL);
	struct tm *tm = localtime(&rawtime);
	strftime(str, size, "%Y/%m/%d %H:%M:%S", tm);
}

int init_logging()
{
	int flag = 0;
	struct stat st;
	if (stat("logging", &st) == 0 && S_ISDIR(st.st_mode))
	{
		if (stat("logging/log.txt", &st) == 0 && S_ISREG(st.st_mode))
		{
			printf("log file already exists\n");
			flag = 1;
			return flag;
		}
		return flag;
	}

	if (mkdir("logging", S_IRWXU) != 0)
	{
		perror("mkdir");
		return -1;
	}
	return flag;
}

void log_result(struct Hashtable *t, int index, int lflag)
{
	FILE *log_file;
	if (lflag == 0)
	{
		log_file = fopen("logging/log.txt", "w");
	}
	else
	{
		log_file = fopen("logging/log.txt", "a");
	}
	char tm[50];
	get_timestamp(tm, sizeof(tm));
	fprintf(log_file, "%s %s %s\n", tm, t->records[index]->filename, t->records[index]->hashval);
	fclose(log_file);
}