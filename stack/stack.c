#include "stack.h"

#define CANARY 0x6666666666666666ull
#define POISON 0xFFu

typedef unsigned long long canary_t;
typedef unsigned long long hash_t;


struct Stack
{
	canary_t head;
	size_t elemsize;
	size_t capacity;
	size_t size;
	void *data;
	int errcount;
	hash_t struct_hash;
	hash_t data_hash;
	canary_t tail;
};

int    convert_byte   (char *dst, char c);
int    convert_nbytes (char *dst, const void *ptr, size_t nbytes);

int __stack_ctor (stack stk, size_t capacity, size_t elemsize)
{
	if (!stk || !capacity || !elemsize)
		return 1; /* Bad parameters */

	stk->data = calloc (capacity * elemsize + 2 * sizeof (canary_t), 1);

	if (!stk->data)
		return 1; /* Trouble with calloc */

	stk->head    = CANARY;
	stk->elemsize = elemsize;
	stk->capacity = capacity;
	stk->size     = 0;
	stk->errcount = 0;
	stk->tail   = CANARY;

	*( (canary_t *) stk->data ) = CANARY;
	stk->data += sizeof (canary_t);
	*( (canary_t *) stk->data ) = CANARY;

	for (size_t i = 0; i < stk->capacity; i++)
		for (size_t j = 0; j < stk->elemsize; j++)
			*( (unsigned char *) (stk->data + sizeof (canary_t) + i * stk->elemsize + j) ) = POISON;

	stk->struct_hash = get_hash ((void *) stk + sizeof (canary_t), sizeof (struct Stack) - 2 * sizeof (hash_t) - 2 * sizeof (canary_t) );
	stk->data_hash   = get_hash (stk->data, stk->size * stk->elemsize);

	return 0; /* Success! */
}

int __stack_push (stack stk, const void *value)
{
	if (!stk || !value)
		return 1; /* Bad parameters */

	if (stk->size >= stk->capacity)
	{
		void *tmp = realloc (stk->data - sizeof (canary_t), stk->capacity * 2 * stk->elemsize + 2 * sizeof (canary_t) ); 

		if (tmp)
		{
			stk->capacity *= 2;
			stk->data = tmp + sizeof (canary_t);

			for (size_t i = stk->size; i < stk->capacity; i++)
				for (size_t j = 0; j < stk->elemsize; j++)
					*( (unsigned char *) (stk->data + sizeof (canary_t) + i * stk->elemsize + j) ) = POISON;
		}
		else
			return 1; /* Trouble witch realloc */
	}

	for (size_t i = 0; i < stk->elemsize; i++)
		*( (char*) stk->data + stk->elemsize * stk->size + i) = *( (const char*) value + i);
	
	stk->size++;

	*( (canary_t *) (stk->data + stk->elemsize * stk->size) ) = CANARY;

	stk->struct_hash = get_hash ((void *) stk + sizeof (canary_t), sizeof (struct Stack) - 2 * sizeof (hash_t) - 2 * sizeof (canary_t) );
	stk->data_hash   = get_hash (stk->data, stk->size * stk->elemsize);

	return 0; /* Success! */
}

int __stack_pop (stack stk, void *value)
{
	if (!stk || !value || __stack_is_empty (stk) )
		return 1; /* Bad parameters */

	for (size_t i = 0; i < sizeof (canary_t); i++)
		*( (unsigned char *) (stk->data + stk->size * stk->elemsize + i) ) = POISON;

	stk->size--;

	for (size_t i = 0; i < stk->elemsize; i++)
		*( (char *) value + i) = *( (char *) stk->data + stk->elemsize * stk->size + i);

	*( (canary_t *) (stk->data + stk->elemsize * stk->size) ) = CANARY;

	stk->struct_hash = get_hash ((void *) stk + sizeof (canary_t), sizeof (struct Stack) - 2 * sizeof (hash_t) - 2 * sizeof (canary_t) );
	stk->data_hash   = get_hash (stk->data, stk->size * stk->elemsize);

	return 0; /* Success! */
}

int __stack_dtor (stack *stk)
{
	if (!stk || !(*stk) || !(*stk)->data)
		return 1; /* Bad parameters */

	free ( (*stk)->data - sizeof (canary_t) );
	free (*stk);
	*stk = NULL;

	return 0; /* Success */
}

int __stack_check (stack stk)
{
	if (!stk)
		return -1; /* Bad parameters */

	int errcount     = 0;
	long long errors = 0;

	if (stk->struct_hash != get_hash ((void *) stk + sizeof (canary_t), sizeof (struct Stack) - 2 * sizeof (hash_t) - 2 * sizeof (canary_t) ) )
	{
		errcount++;
		errors |= 1 << 0;
		__stack_errors_log (stk, errors);
		stk->errcount = errcount;
		return errcount;
	}

	if (stk->head != CANARY)
	{
		errcount++;
		errors |= 1 << 1;
	}

	if (stk->tail != CANARY)
	{
		errcount++;
		errors |= 1 << 2;
	}

	if (stk->data_hash != get_hash (stk->data, stk->elemsize * stk->size) )
	{
		errcount++;
		errors |= 1 << 3;
	}
	
	if ( *( (canary_t *) (stk->data - sizeof (canary_t) ) ) != CANARY) 
	{
		errcount++;
		errors |= 1 << 4;
	}

	if ( *( (canary_t *) (stk->data + stk->size * stk->elemsize) ) != CANARY)
	{
		errcount++;
		errors |= 1 << 5;
	}

	if (errcount)
		__stack_errors_log (stk, errors);

	stk->errcount = errcount;

	return errcount;
}

int __stack_errors_log (stack stk, long long errors)
{
	if (!stk)
		return -1; /* Bad parameters */

	FILE *log = fopen ("stack_errors_log", "w");

	fprintf (log, "stack_check log (stack adres is %p):\n", stk);

	int errcount = 0;
	for (size_t i = 0; i < sizeof (errors); i++)
	{
		unsigned char errcode = (unsigned char) ( (errors & (1 << i) ) >> i) * (i + 1);

		if (errcode)
			errcount++;

		switch (errcode)
		{
			case 0:
				break;
			case 1:
				fprintf (log, "\tERROR[%d]: structure hash has changed;\n", errcount);
				fclose (log);
				return errcount;

			case 2:
				fprintf (log, "\tERROR[%d]: head structure canary has changed;\n", errcount);
				break;

			case 3:
				fprintf (log, "\tERROR[%d]: tail structure canary has changed;\n", errcount);
				break;

			case 4:
				fprintf (log, "\tERROR[%d]: data hash has changed;\n", errcount);
				break;

			case 5:
				fprintf (log, "\tERROR[%d]: head data canary has changed;\n", errcount);
				break;

			case 6:
				fprintf (log, "\tERROR[%d]: tail data canary has changed;\n", errcount);
				break;

			default:
				return -1;
		}
	}

	fclose (log);

	return errcount;
}

int __stack_is_empty (stack stk)
{
	if (!stk)
		return 1;

	if (!stk->size)
		return 1;
	else
		return 0;
}

size_t __stack_elemsize (stack stk)
{
	if (!stk)
		return 0;
	else
		return stk->elemsize; 
}

size_t __struct_Stack_size ()
{
	return sizeof (struct Stack);
}

int __stack_errcount (stack stk)
{
	if (!stk)
		return -1;

	return stk->errcount;
}

size_t stack_size (stack stk)
{
	if (!stk)
		return 0;
	else
		return stk->size;
}
