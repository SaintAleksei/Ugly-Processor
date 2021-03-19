#include "tools.h"

void *super_fread (const char *fname, size_t elemsize, size_t *count)
{
	if (!elemsize || !fname)
		return NULL;

	FILE *stream = fopen (fname, "rb");

	if (!stream)
	{
		fprintf (stderr, "super_fread: Can't open file '%s'\n", fname);

		return NULL;
	}

	fseek (stream, 0, SEEK_END);
	size_t fsize = ftell (stream);
	fseek (stream, 0, SEEK_SET);

	fsize /= elemsize;

	void *data = calloc (fsize, elemsize);

	if (fread (data, elemsize, fsize, stream) != fsize)
	{
		fprintf (stderr, "super_fread: Can't read data from file '%s'\n", fname);

		free (data);

		return NULL;
	}
	
	if (count)
		*count = fsize;

	fclose (stream);

	return data;
}

int change_format (char *str, const char *format)
{
	if (!str || !format)
		return -1;

	char *ptr = strchr (str, '.');

	if (ptr)
	{
		*ptr = 0;
		sprintf (ptr, "%s", format);
	}
	else
		sprintf (str + strlen (str), "%s", format);

	return 0;
}

hash_t get_hash (const void *ptr, size_t nbytes)
{
	hash_t result = 0;	
	hash_t pow    = 1;

	for (size_t i = 0; i < nbytes; i++, pow *= 997)
		result += *( (const unsigned char *) ptr + i) * pow;

	return result;
}
