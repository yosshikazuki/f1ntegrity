#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utility.h"

void sstrcpy(char *dest, const char *src, size_t size)
{
	while (*src != '\0' && size > 1)
	{
		*dest++ = *src++;
		size--;
	}

	*dest = '\0';
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
			if (*a++ != *b++)
			{
				b = substring;
				break;
			}
		}
	}
	return NULL;
}

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
