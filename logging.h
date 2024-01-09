#ifndef LOGGING_H
#define LOGGING_H

void get_timestamp(char str[], size_t size);
int init_logging();
void log_result(struct Hashtable *t, int index, int lflag);

#endif