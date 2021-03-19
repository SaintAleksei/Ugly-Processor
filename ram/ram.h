#ifndef RAM_H_INCLUDED
#define RAM_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../tools/tools.h"
#include "../commands.h"

#ifndef DATA_SIZE
#define DATA_SIZE 4096
#endif

typedef struct RAM
{
	cmd_t *memory;
	size_t text_size;
	size_t data_size;
	unsigned char status;
	hash_t text_hash;
	hash_t struct_hash;
} ram_t;

enum RAM_STATUSES
{
	RAM_DESTROYED,
	RAM_CREATED,
	RAM_OK,
	RAM_TEXT_ERROR,
	RAM_STRUCT_ERROR,
};

int ram_create    (ram_t *ram, size_t text_size, size_t data_size);
int ram_load      (ram_t *ram, const cmd_t *buffer);
int ram_destroy   (ram_t *ram);
int ram_read      (ram_t *ram, cmd_t addr, cmd_t *dest);
int ram_write     (ram_t *ram, cmd_t addr, cmd_t src);
int ram_check     (ram_t *ram);
int ram_error     (ram_t *ram);
int ram_log       (ram_t *ram, FILE *stream);
int ram_dump      (ram_t *ram, FILE *stream);

#endif
