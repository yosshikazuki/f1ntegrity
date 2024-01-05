#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fparam.h"

void sstrcpy(char *dest, char *src, size_t size)
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