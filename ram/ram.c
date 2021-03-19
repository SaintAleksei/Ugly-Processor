#include "ram.h"

int ram_create (ram_t *ram, size_t text_size, size_t data_size)
{
	if (!ram || !text_size || ram->status != RAM_DESTROYED)
		return __LINE__;

	ram->memory = calloc (text_size + data_size, sizeof (cmd_t) );

	if (!ram->memory)
		return __LINE__;

	ram->text_size = text_size;
	ram->data_size = data_size;
	ram->status    = RAM_CREATED;

	return 0;
}

int ram_load (ram_t *ram, const cmd_t *buffer)
{
	if (!ram || !ram->memory || ram->status != RAM_CREATED || !buffer)
		return __LINE__;

	memcpy (ram->memory, buffer, ram->text_size * sizeof (cmd_t) );

	ram->status      = RAM_OK;
	ram->text_hash   = get_hash (ram->memory, ram->text_size * sizeof (cmd_t) );
	ram->struct_hash = get_hash (ram, sizeof (ram_t) - sizeof (hash_t) );

	return 0;
}


int ram_destroy (ram_t *ram)
{
	if (!ram)
		return __LINE__;

	free (ram->memory);

	memset (ram, 0, sizeof (ram_t) );

	ram->status = RAM_DESTROYED;

	return 0;
}

int ram_read (ram_t *ram, cmd_t addr, cmd_t *dest)
{
	if (!ram || addr >= ram->text_size + ram->data_size || !dest)
		return __LINE__;

	*dest = ram->memory[addr];

	return 0;
}

int ram_write (ram_t *ram, cmd_t addr, cmd_t src)
{
	if (!ram || addr >= ram->text_size + ram->data_size)
		return __LINE__;

	ram->memory[addr] = src;

	return 0;
}

int ram_check (ram_t *ram)
{
	if (!ram)
		return __LINE__;

	if (ram->struct_hash != get_hash (ram, sizeof (ram_t) - sizeof (hash_t) ) || !ram->memory)
	{
		ram->status = RAM_STRUCT_ERROR;

		return __LINE__;
	}

	if (ram->text_hash != get_hash (ram->memory, ram->text_size * sizeof (cmd_t) ) )
	{
		ram->status = RAM_TEXT_ERROR;

		return __LINE__;
	}


	return 0;
}

int ram_error (ram_t *ram)
{
	if (!ram)
		return __LINE__;

	switch (ram->status)
	{
		case RAM_TEXT_ERROR:
			fprintf (stderr, "RAM: [ERROR]: Data in text section has been changed: proccessor can't continue running\n");
			break;

		case RAM_STRUCT_ERROR:
			fprintf (stderr, "RAM: [ERROR]: RAM structure has been changed: proccessor can't continue running\n");
			break;
		
		default:
			return 0;
	}
		
	return 0;
}

int ram_log (ram_t *ram, FILE *stream)
{
	if (!ram || !stream) 
		return __LINE__;

	fprintf (stream, "----------$$$$$ RAM dump (%p) $$$$$----------\n"
					 "\n"
					 "Structure:\n"
					 "{\n"
					 "\ttext_size   = %lu;\n"
					 "\tdata_size   = %lu;\n"
					 "\tstatus      = %hhu;\n"
					 "\ttext_hash   = 0x%08llx;\n"
					 "\t\n"
					 "\thash is 0x%08llx;\n"
					 "\t\n"
			  		 "\tstruct_hash = 0x%08llx;\n"
					 "}\n"
					 "\n", ram, ram->text_size, ram->data_size, ram->status, ram->text_hash,
						   get_hash (ram, sizeof (ram_t) - sizeof (hash_t) ), ram->struct_hash);

	fprintf (stream, "Text section:\n"
					 "{\n");

	for (size_t i = 0; i < ram->text_size; i++)
		fprintf (stream, "\t[%02lu] = 0x%08llx;\n", i, ram->memory[i]);

	fprintf (stream, "\t\n"
					 "\thash is 0x%08llx;\n"
					 "}\n"
					 "\n", get_hash (ram->memory, ram->text_size * sizeof (cmd_t) ) );
	return 0;
}

int ram_dump (ram_t *ram, FILE *stream)
{
	if (!ram || !ram->memory || ram->status == RAM_STRUCT_ERROR || !stream)
		return __LINE__;

	fwrite (ram->memory, sizeof (cmd_t), ram->text_size, stream);
	
	return 0;
}
