#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "fparam.h"
#include "logging.h"
#include "utility.h"

void check_file_integrity(struct Hashtable *t, const char *arg, char *fname, int lflag)
{
	char filename[MAXLENGTH]; 
	char hash_0x[HASHSIZE], hash_1x[HASHSIZE];
	CreateHash hash[] = {&file_hash, &string_hash};
	if (fname == NULL)
	{
		sstrcpy(filename, arg, MAXLENGTH); 
	}
	else
	{
		arg[-1] == '/' ? snprintf(filename, sizeof(filename), "%s/%s", arg, fname) : snprintf(filename, sizeof(filename), "%s%s", arg, fname);
	}
	FILE *file = fopen(filename, "rb");
	hash[0](file, hash_0x);
	hash[1](filename, hash_1x);
	int byte_size = strlen(hash_0x) / 2;
	int index = create_index(t, hash_1x, byte_size);
	t = insert_record(t, filename, hash_0x, index);
	log_result(t, index, lflag);
}

void search_dir(struct Hashtable *t, const char *dirname, int lflag)
{
	struct dirent *dentry;
	DIR *dir;
	dir = opendir(dirname);
	while ((dentry = readdir(dir)) != NULL)
	{
		if (dentry->d_type == DT_REG)
		{
			check_file_integrity(t, dirname, dentry->d_name, lflag);
		}
	}
}

void validate_input(const char *arg, const char *arg2)
{
	// check if -r is provided as arg. if it is, then check if arg2 is a directory. if not, check if arg is a file.
	struct stat fstat;
	if (strcmp(arg, "-r") == 0)
	{
		// check if arg2 is null
		if (arg2 == NULL)
		{
			print_error();
			exit(0);
		}

		// check if arg2 is a directory
		stat(arg2, &fstat);
		if (S_ISDIR(fstat.st_mode) == 1)
		{
			return;
		}
	}
	else
	{
		// check if arg is a file
		stat(arg, &fstat);
		if (S_ISREG(fstat.st_mode) != 1)
		{
			print_error();
			exit(0);
		}
	}
}

void print_error()
{
	printf("Usage: ./f1ntegrity /path/to/target/file\n");
	printf("       ./f1ntegrity -r /path/to/target/directory\n");
	exit(0);
}

struct Hashtable *insert_record(struct Hashtable *t, char filename[], char hashval[], int index)
{
	struct Fparam *new_record;
	new_record = (struct Fparam *)malloc(sizeof(struct Fparam));
	sstrcpy(new_record->filename, filename, sizeof(new_record->filename));
	sstrcpy(new_record->hashval, hashval, sizeof(new_record->hashval));
 
	t->records[index] = new_record;
	compare_hash(filename, hashval);
	
	return t;
}

void compare_hash(char *filename, char *hashval)
{

	FILE *file = fopen("logging/log.txt", "r");
	char *buffer = NULL;
	size_t bufsize = 0;
	int line_count = 0;
	int modified = 0, unmodified = 0;
	while (getline(&buffer, &bufsize, file) != -1)
	{
		line_count++;
		char *filename_search = sstrstr(buffer,filename);
		char *hashval_search = sstrstr(buffer, hashval);
		
		if (filename_search != NULL && hashval_search != NULL)
		{
			char temp_filename[strlen(filename_search)+1];
			char temp_hashval[strlen(hashval_search)+1];
			
			sstrcpy(temp_filename, filename_search, sizeof(temp_filename));
			sstrcpy(temp_hashval, hashval_search, sizeof(temp_hashval));

			int filename_match = strcmp(temp_filename, filename) == 0;
			int hashval_match = strcmp(temp_hashval, hashval) == 0;

			unmodified = filename_match && hashval_match;
		}
		else if (filename_search != NULL)
		{
			char temp_filename[strlen(filename_search)+1];
			sstrcpy(temp_filename, filename_search, sizeof(temp_filename));
			if (strcmp(temp_filename, filename) == 0)
			{
				modified = 1;
			}
		}
	}

	if (modified)
	{
		printf("file modified!!\n");
	}
	else if (!modified && unmodified)
	{
		printf("file found unmodified\n");
	}
	else
	{
		printf("first appearance\n");
	}

	free(buffer);
	fclose(file);
}

struct Hashtable *create_table()
{
	struct Hashtable *new_table = (struct Hashtable *)malloc(sizeof(struct Hashtable));
	for (int i = 0; i < TABLESIZE; i++)
	{
		new_table->records[i] = NULL;
	}

	return new_table;
}

int create_index(struct Hashtable *t, char *hashval, int byte_size)
{
	int index = 0;
	for (int i = 0; i < byte_size; i++)
	{
		index = (index * 256 + hashval[i]) % TABLESIZE; 
	}
	while (t->records[index] != NULL)
	{
		index = (index + 1) % TABLESIZE;
		if (index >= TABLESIZE) 
		{
			perror("index collision");
			exit(1);
		}
	}

	return index;
}
