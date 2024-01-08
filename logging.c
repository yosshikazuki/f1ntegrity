#include <time.h>
#include <sys/stat.h>
#include <stdio.h>


void get_timestamp(char str[], size_t size)
{
	time_t rawtime;
	rawtime = time(NULL);
	struct tm *tm = localtime(&rawtime);
	strftime(str, size, "%Y/%m/%d %H:%M:%S", tm);
}

void init_logging()
{
	struct stat st;
	if (stat("logging", &st) == 0 && S_ISDIR(st.st_mode))
	{
		return;
	}

	if (mkdir("logging", S_IRWXU) != 0)
	{
		perror("mkdir");
	}

	FILE *log_file = fopen("logging/log.txt", "w");
	char t[50];
	get_timestamp(t, sizeof(t));
	fprintf(log_file, "%s\n", t);
}
