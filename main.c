#include <stdio.h>
#include <string.h>
#include "fparam.h"
#include "logging.h"
#include "graphics.h"

int main(int argc, char **argv)
{
	struct Hashtable *file_list = NULL;
	file_list = create_table();
	int lflag;

	switch (argc)
	{
		case 2:
			validate_input(argv[1], NULL);
			lflag = init_logging();
			check_file_integrity(file_list, argv[1], NULL, lflag);
			init_window(argc, argv);
			break;
	
		case 3:
			validate_input(argv[1], argv[2]);
			lflag = init_logging();
			search_dir(file_list, argv[2], lflag);
			break;
	
		default:
			printf("switch statement\n");
			print_error();
	}

	return 0;
}