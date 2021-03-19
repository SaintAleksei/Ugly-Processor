#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long hash_t;

void *super_fread  (const char *fname, size_t elemsize, size_t *count);
int  change_format (char *str, const char *format);
hash_t get_hash    (const void *ptr, size_t nbytes);

#endif
