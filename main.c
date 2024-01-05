#include <stdio.h>
#include <string.h>
#include "fparam.h"

int main(int argc, char **argv)
{
	struct Hashtable *file_list = NULL;
	file_list = create_table();

	switch (argc)
	{
		case 2:
			validate_input(argv[1], NULL);
			check_file_integrity(file_list, argv[1], NULL);
			break;
	
		case 3:
			validate_input(argv[1], argv[2]);
			search_dir(file_list, argv[2]);
			break;
	
		default:
			printf("Usage: ./f1ntegrity /path/to/target/file\n");
			printf("       ./f1ntegrity -r /path/to/target/directory\n");
			exit(0);
	}
	char t[50];
	get_timestamp(t, sizeof(t));
	printf("%s\n", t);
	return 0;
}