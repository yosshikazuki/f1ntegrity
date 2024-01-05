#include <stdio.h>
#include <string.h>
#include "fparam.h"
#include <time.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: ./f1ntegrity /path/to/target/file\n");
		exit(0);
	}

	struct Hashtable *file_list = NULL;
	file_list = create_table();

	FILE *fp;
	char filename[MAXLENGTH];
	sstrcpy(filename, argv[1], MAXLENGTH); // todo
	fp = fopen(filename, "rb");
	char hash_0x[EVP_MAX_MD_SIZE * 2 + 1];
	create_hash(fp, hash_0x);
	fclose(fp);

	time_t rawtime;
	rawtime = time(NULL);
	struct tm *timestamp = localtime(&rawtime);
	printf("%s", asctime(timestamp));
	int byte_size = strlen(hash_0x) / 2;
	int index = create_index(file_list, hash_0x, byte_size);
	printf("index: %d (%d byte SHA256)\n", index, byte_size);
	
	file_list = insert_record(file_list, filename, hash_0x, index);
	printf("%s: %s\n", file_list->records[index]->filename, file_list->records[index]->hashval);

	return 0;
}