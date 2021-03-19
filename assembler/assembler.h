#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../tools/tools.h"
#include "../commands.h"

#define MAX_LABEL_SIZE 64
#define MAX_BUFFER_SIZE 128

typedef struct label 
{
	char name[MAX_LABEL_SIZE];
	cmd_t addr;
	size_t line;
} label_t;

cmd_t get_cmd        (const char *str);
cmd_t get_push_arg   (const char *str, cmd_t cmd);
cmd_t get_pop_arg    (const char *str, cmd_t cmd);
int   get_jmp_arg    (char *dest, const char *src);
int   get_label      (char *dest, const char *src);
cmd_t find_label     (label_t *labels, size_t labels_count, const char *name);

#endif
