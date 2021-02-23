#include "stack.h"

#define CANARY 0x6666666666666666ull

typedef unsigned long long canary_t;

struct Stack
{
	canary_t start;
	size_t elemsize;
	size_t capacity;
	size_t size;
	void *data;
	int errcount;
	canary_t finish;
};

int __stack_ctor (stack stk, size_t capacity, size_t elemsize)
{
	if (!stk || !capacity || !elemsize)
		return 1; /* Bad parameters */

	stk->data = calloc (capacity * elemsize + 2 * sizeof (canary_t), 1);

	if (!stk->data)
		return 1; /* Trouble with calloc */

	stk->start    = CANARY;
	stk->elemsize = elemsize;
	stk->capacity = capacity;
	stk->size     = 0;
	stk->errcount = 0;
	stk->finish   = CANARY;

	*( (canary_t *) stk->data ) = CANARY;
	stk->data += sizeof (canary_t);
	*( (canary_t *) stk->data ) = CANARY;

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
		}
		else
			return 1; /* Trouble witch realloc */
	}

	for (size_t i = 0; i < stk->elemsize; i++)
		*( (char*) stk->data + stk->elemsize * stk->size + i) = *( (const char*) value + i);
	
	stk->size++;

	*( (canary_t *) (stk->data + stk->elemsize * stk->size) ) = CANARY;

	return 0; /* Success! */
}

int __stack_pop (stack stk, void *value)
{
	if (!stk || !value || __stack_is_empty (stk) )
		return 1; /* Bad parameters */

	stk->size--;

	for (size_t i = 0; i < stk->elemsize; i++)
		*( (char *) value + i) = *( (char *) stk->data + stk->elemsize * stk->size + i);

	*( (canary_t *) (stk->data + stk->elemsize * stk->size) ) = CANARY;

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

	int errcount = 0;

	if (!(stk->data) )
		errcount++;

	if (!(stk->capacity) )
		errcount++;

	if (stk->start != CANARY)
		errcount++;

	if (stk->finish != CANARY)
		errcount++;
	
	if ( *( (canary_t *) (stk->data - sizeof (canary_t) ) ) != CANARY) 
		errcount++;

	if ( *( (canary_t *) (stk->data + stk->size * stk->elemsize) ) != CANARY)
		errcount++;

	if (errcount)
		__stack_check_log (stk);

	stk->errcount = errcount;

	return errcount;
}

int __stack_check_log (stack stk)
{
	if (!stk)
		return -1; /* Bad parameters */

	FILE *log = fopen ("stack_check_log", "w");

	fprintf (log, "stack_check log (stack adres is %p):\n", stk);

	int errcount = 0;

	if (!(stk->data) )
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: stk->data is NULL;\n", errcount);
	}

	if (!(stk->capacity) )
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: stk->capacity is 0;\n", errcount);
	}

	if (stk->start != CANARY)
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: first structure canary (stk->start) has changed (%llx);\n", errcount, stk->start);
	}

	if (stk->finish != CANARY)
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: second structure canary (stk->finish) has changed (%llx);\n", errcount, stk->finish);
	}
	
	if ( *( (canary_t *) (stk->data - sizeof (canary_t) ) ) != CANARY) 
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: first data canary has changed (%llx);\n", 
					   errcount, *( (canary_t *) (stk->data - sizeof (canary_t) ) ) );
	}

	if ( *( (canary_t *) (stk->data + stk->size * stk->elemsize) ) != CANARY)
	{
		errcount++;
		fprintf (log, "\tERROR[%d]: second data canary has changed (%llx);\n", 
					   errcount, *( (canary_t *) (stk->data + stk->size * stk->elemsize) ) );
	}

	fclose (log);

	return 0;
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

int __stack_errors (stack stk)
{
	if (!stk)
		return -1;

	return stk->errcount;
}
