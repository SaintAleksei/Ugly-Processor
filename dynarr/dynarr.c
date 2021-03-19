#include "dynarr.h"

int __arr_create (arr_t *ptr, size_t elemsize, size_t line, const char *fname, const char *vname)
{
	if (!ptr || !elemsize)
	{
		fprintf (stderr, "@ [ERROR] %s:%lu: arr_create (%s, %lu): invalid parameters\n", 
						 fname, line, vname, elemsize);
		return ARR_ERROR;
	}

	ptr->arr = calloc (1024, 1);
	
	if (!ptr->arr)
	{
		fprintf (stderr, "@ [ERROR] %s:%lu: arr_create (%s, %lu): trouble with calloc\n", 
						 fname, line, vname, elemsize);
		return ARR_ERROR;
	}

	ptr->capacity = 1024;
	ptr->elemsize = elemsize;	
	ptr->counter  = 0;

	return 0;
}

int __arr_destroy (arr_t *ptr, size_t line, const char *fname, const char *vname)
{
	if (!ptr)
	{
		fprintf (stderr, "@ [ERROR] %s:%lu: arr_destroy (%s): invalid parameter\n", 
						 fname, line, vname);
		return ARR_ERROR;
	}

	free (ptr->arr);	

	ptr->arr      = NULL;
	ptr->capacity = 0;
	ptr->counter  = 0;
	ptr->elemsize = 0;

	return 0;
}

int __arr_x2expand (arr_t *ptr, size_t line, const char *fname, const char *vname)
{
	if (!ptr)
	{
		fprintf (stderr, "@ [ERROR] %s:%lu: arr_x2expand (%s): invalid parameter\n", 
						 fname, line, vname);
		return ARR_ERROR;
	}

	void *tmp = realloc (ptr->arr, ptr->capacity);

	if (tmp)
	{
		ptr->arr = tmp;
		ptr->capacity *= 2;

		for (size_t i = ptr->counter; i < ptr->capacity; i++)
			for (size_t j = 0; j < ptr->elemsize; j++)
				*( (char *) (ptr->arr + i + j) ) = 0;
	
		return 0;
	}	
	else
	{
		fprintf (stderr, "@ [ERROR] %s:%lu: arr_x2expand (%s): trouble with realloc\n", 
						 fname, line, vname);
		return ARR_ERROR;
	}
}
