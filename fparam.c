#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include "fparam.h"


void check_file_integrity(struct Hashtable *t, const char *arg, char *fname)
{
	char filename[MAXLENGTH]; 
	char hash_0x[EVP_MAX_MD_SIZE * 2 + 1];
	if (fname == NULL)
	{
		sstrcpy(filename, arg, MAXLENGTH); 
	}
	else
	{
		snprintf(filename, sizeof(filename), "%s%s", arg, fname);
	}
	FILE *file = fopen(filename, "rb");
	create_hash(file, hash_0x);
	int byte_size = strlen(hash_0x) / 2;
	int index = create_index(t, hash_0x, byte_size);
	printf("index: %d (%d byte SHA256)\n", index, byte_size);
	t = insert_record(t, filename, hash_0x, index);
	printf("%s: %s\n", t->records[index]->filename, t->records[index]->hashval);
}

void search_dir(struct Hashtable *t, const char *dirname)
{
	struct dirent *dentry;
	DIR *dir;
	dir = opendir(dirname);
	while ((dentry = readdir(dir)) != NULL)
	{
		if (dentry->d_type == DT_REG)
		{
			check_file_integrity(t, dirname, dentry->d_name);
		}
	}
}


void validate_input(const char *arg, const char *arg2)
{

	if (strcmp(arg, "-r") == 0)
	{
		assert(arg2 != NULL);
		return;
	}

	struct stat fstat;
	stat(arg, &fstat);
	if (S_ISREG(fstat.st_mode) != 1)
	{
		printf("Usage: ./f1ntegrity /path/to/target/file\n");
		exit(1);
	}
}

void sstrcpy(char *dest, const char *src, size_t size)
{
	while (*src != '\0' && size > 1)
	{
		*dest++ = *src++;
		size--;
	}

	*dest = '\0';
}


struct Hashtable *insert_record(struct Hashtable *t, char filename[], char hashval[], int index)
{
	struct Fparam *new_record;
	new_record = (struct Fparam *)malloc(sizeof(struct Fparam));
	sstrcpy(new_record->filename, filename, sizeof(new_record->filename));
	sstrcpy(new_record->hashval, hashval, sizeof(new_record->hashval));

	t->records[index] = new_record;

	return t;
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


void create_hash(FILE *file, char *str)
{
	EVP_MD_CTX *mdctx;
	const EVP_MD *md = EVP_sha256();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	char buffer[4096];
	size_t bytes_read;

	OpenSSL_add_all_digests();

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
	{
		EVP_DigestUpdate(mdctx, buffer, bytes_read);
	}

	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	for (int i = 0; i < md_len; i++)
	{
		sprintf(str + 2 * i, "%02x", md_value[i]); // todo
	}
	EVP_cleanup();
}


void get_timestamp(char str[], size_t size)
{
	time_t rawtime;
	rawtime = time(NULL);
	struct tm *tm = localtime(&rawtime);
	strftime(str, size, "%Y/%m/%d %H:%M:%S", tm);
}