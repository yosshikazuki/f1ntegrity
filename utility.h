#ifndef UTILITY_H
#define UTILITY_H

#include "/opt/homebrew/Cellar/openssl@3/3.2.0/include/openssl/evp.h"

typedef void (*CreateHash)(void *, char *);

#define HASHSIZE EVP_MAX_MD_SIZE * 2 + 1

void sstrcpy(char *dest, const char *src, size_t size);
char *sstrstr(char *string, char *substring);
void file_hash(void *file, char *str);
void string_hash(void *input, char *str);

#endif