#ifndef FPARAM_H
#define FPARAM_H

#include "/opt/homebrew/Cellar/openssl@3/3.2.0/include/openssl/evp.h"

#define MAXLENGTH 100
#define TABLESIZE 100

struct Fparam
{
	char filename[100];
	char hashval[EVP_MAX_MD_SIZE*2+1];
};

struct Hashtable
{
	struct Fparam *records[TABLESIZE];
};

void sstrcpy(char *dest, char *src, size_t size);
struct Hashtable *insert_record(struct Hashtable *t, char filename[], char hashval[], int index);
struct Hashtable *create_table();
int create_index(struct Hashtable *t, char *hashval, int byte_size);
void create_hash(FILE *file, char *str);

#endif