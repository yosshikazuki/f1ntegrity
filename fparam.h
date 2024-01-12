#include "utility.h"

#ifndef FPARAM_H
#define FPARAM_H

#define MAXLENGTH 100
#define TABLESIZE 100

#define MODMSG "file modified!!"
#define UNMODMSG "file found unmodified"
#define DEFMSG "first appearance"

struct Fparam
{
	char filename[MAXLENGTH];
	char hashval[HASHSIZE];
};

struct Hashtable
{
	struct Fparam *records[TABLESIZE];
};

struct Hashtable *insert_record(struct Hashtable *t, char filename[], char hashval[], int index);
struct Hashtable *create_table();
int create_index(struct Hashtable *t, char *hashval, int byte_size);
void validate_input(const char *arg, const char *arg2);
void print_error();
void search_dir(struct Hashtable *t, const char *dirname, int lflag);
void check_file_integrity(struct Hashtable *t, const char *arg, char *fname, int lflag);
void compare_hash(char *filename, char *hashval);

#endif