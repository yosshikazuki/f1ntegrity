#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "fparam.h"
#include "logging.h"

/**
 * @brief Checks the integrity of a file and updates the hashtable.
 *
 * This function checks the integrity of a specified file by calculating its SHA256 hash.
 * It then updates the provided hashtable with information about the file, such as filename,
 * hash value, and index. Additionally, it prints information about the file and logs the result
 * based on the specified logging flag.
 *
 * @param t A pointer to the hashtable storing file-related information.
 * @param arg The directory path or file path, depending on the context.
 * @param fname The filename (if provided, used in combination with arg to form the complete path).
 * @param lflag The logging flag indicating whether to create or append to the logging file.
 */
void check_file_integrity(struct Hashtable *t, const char *arg, char *fname, int lflag)
{
	char filename[MAXLENGTH]; 
	char hash_0x[EVP_MAX_MD_SIZE * 2 + 1], hash_1x[EVP_MAX_MD_SIZE * 2 + 1];
	CreateHash hash[] = {&file_hash, &string_hash};
	if (fname == NULL)
	{
		sstrcpy(filename, arg, MAXLENGTH); 
	}
	else
	{
		arg[-1] == '/' ? snprintf(filename, sizeof(filename), "%s%s", arg, fname) : snprintf(filename, sizeof(filename), "%s/%s", arg, fname);
	}
	FILE *file = fopen(filename, "rb");
	hash[0](file, hash_0x);
	hash[1](filename, hash_1x);
	int byte_size = strlen(hash_0x) / 2;
	int index = create_index(t, hash_1x, byte_size);
	printf("index: %d (%d byte SHA256) - %s\n", index, byte_size, hash_1x);
	t = insert_record(t, filename, hash_0x, index);
	printf("%s: %s\n", t->records[index]->filename, t->records[index]->hashval);
	log_result(t, index, lflag);
}

/**
 * @brief Recursively searches a directory for regular files and checks their integrity.
 *
 * This function opens the specified directory and iterates through its contents.
 * For each regular file found, it calls the check_file_integrity function to calculate
 * the SHA256 hash and update the provided hashtable with information about the file.
 * Additionally, it logs the result based on the specified logging flag.
 *
 * @param t A pointer to the hashtable storing file-related information.
 * @param dirname The name of the directory to search.
 * @param lflag The logging flag indicating whether to create or append to the logging file.
 */
void search_dir(struct Hashtable *t, const char *dirname, int lflag)
{
	struct dirent *dentry;
	DIR *dir;
	dir = opendir(dirname);
	while ((dentry = readdir(dir)) != NULL)
	{
		if (dentry->d_type == DT_REG)
		{
			printf("%s is a directory\n", dirname);
			check_file_integrity(t, dirname, dentry->d_name, lflag);
		}
	}
}

/**
 * @brief Validates input arguments for file processing.
 *
 * This function validates the input arguments to ensure they meet the expected criteria
 * for file processing. It checks whether the input corresponds to a regular file and
 * performs additional checks based on the context provided by the arguments.
 * If the validation fails, it prints an error message and exits the program.
 *
 * @param arg The first argument representing a flag or file path.
 * @param arg2 The second argument (optional) providing additional context.
 */
void validate_input(const char *arg, const char *arg2)
{
	// check if -r is provided as arg. if it is, then check if arg2 is a directory. if not, check if arg is a file.
	struct stat fstat;
	if (strcmp(arg, "-r") == 0)
	{
		// check if arg2 is null
		if (arg2 == NULL)
		{
			printf("strcmp\n");
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
			printf("S_ISREG\n");
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


void sstrcpy(char *dest, const char *src, size_t size)
{
	while (*src != '\0' && size > 1)
	{
		*dest++ = *src++;
		size--;
	}

	*dest = '\0';
}

/**
 * @brief Inserts a new record into the hashtable.
 *
 * This function creates a new record with the provided filename, hash value,
 * and index. It then inserts the new record into the specified hashtable.
 *
 * @param t A pointer to the hashtable where the record will be inserted.
 * @param filename The name of the file to be added to the hashtable.
 * @param hashval The hash value corresponding to the file.
 * @param index The index where the record should be inserted in the hashtable.
 * @return A pointer to the hashtable with the new record inserted.
 */
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

char *sstrstr(char *string, char *substring)
{
	register char *a, *b;
	b = substring;
	if (*b == 0)
	{
		return string;
	}
	for (; *string != 0; string++)
	{
		if (*string != *b)
		{
			continue;
		}
		a = string;
		while (1)
		{
			if (*b == 0)
			{
				size_t length = a - (string-1);
				char *result = (char *)malloc(length+1);
				sstrcpy(result, string, length);
				result[length] = '\0';
				return result;
			}
			if (*a++ != *b++) break;
		}
		b = substring;
	}
	return NULL;
}


void compare_hash(char *filename, char *hashval)
{
	FILE *file = fopen("logging/log.txt", "r");
	char *buffer = NULL;
	size_t bufsize = 0;
	int line_count = 0;
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

			if (strcmp(temp_filename, filename) == 0 && strcmp(temp_hashval, hashval) != 0)
			{
				printf("modified file\n");
				break;
			}
			else if (strcmp(temp_filename, filename) == 0 && strcmp(temp_hashval, hashval) == 0)
			{
				printf("found unmodified\n");
			}
			else
			{
				printf("not found\n");
			}
		}
	}
	free(buffer);
	fclose(file);
}

/**
 * @brief Creates a new hashtable for storing file-related information.
 *
 * This function allocates memory for a new hashtable and initializes its
 * records array by setting all elements to NULL. The size of the records array
 * is determined by the TABLESIZE constant.
 *
 * @return A pointer to the newly created hashtable.
 */
struct Hashtable *create_table()
{
	struct Hashtable *new_table = (struct Hashtable *)malloc(sizeof(struct Hashtable));
	for (int i = 0; i < TABLESIZE; i++)
	{
		new_table->records[i] = NULL;
	}

	return new_table;
}

/**
 * @brief Creates an index for inserting a record into the hashtable based on the hash value.
 *
 * This function generates an index by processing each byte of the provided hash value
 * and applying a modular hashing technique. It ensures that the index does not collide with
 * existing records in the hashtable. If a collision occurs, it prints an error message and exits.
 *
 * @param t A pointer to the hashtable where the record will be inserted.
 * @param hashval The hash value used to generate the index.
 * @param byte_size The byte size of the hash value.
 * @return The calculated index for inserting a record into the hashtable.
 */
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

/**
 * @brief Creates a SHA256 hash value for the content of a file.
 *
 * This function reads the content of the specified file and calculates its SHA256 hash value.
 * The hash value is stored as a hexadecimal string in the provided str buffer.
 *
 * @param file A pointer to the opened file for which the hash is calculated.
 * @param str A buffer to store the resulting SHA256 hash as a hexadecimal string.
 */
void file_hash(void *file, char *str)
{
	EVP_MD_CTX *mdctx;
	const EVP_MD *md = EVP_sha256();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	char buffer[1024];
	size_t bytes_read;

	OpenSSL_add_all_digests();

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	while ((bytes_read = fread(buffer, 1, sizeof(buffer), (FILE *)file)) > 0)
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

void string_hash(void *input, char *str)
{
	EVP_MD_CTX *mdctx;
	const EVP_MD *md = EVP_sha256();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;

	OpenSSL_add_all_digests();

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, input, strlen(input));


	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	for (int i = 0; i < md_len; i++)
	{
		sprintf(str + 2 * i, "%02x", md_value[i]); // todo
	}
	EVP_cleanup();
}
