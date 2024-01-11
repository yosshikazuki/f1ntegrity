#include <string.h>
#include <stdlib.h>

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
			if (*a++ != *b++) break;
		}
		b = substring;
	}
	return NULL;
}
