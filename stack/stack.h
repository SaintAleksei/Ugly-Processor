#ifndef STACK_H_INCLUDED

	#define STACK_H_INCLUDED 1

	#include <stdio.h>
	#include <stdlib.h>

	typedef struct Stack * stack;

	int __stack_ctor       (stack stk, size_t capacity, size_t elemsize);
	int __stack_dtor       (stack *stk);
	int __stack_push       (stack stk, const void *value);
	int __stack_pop        (stack stk, void *value);
	int __stack_is_empty   (stack stk);
	int __stack_check      (stack stk);
	int __stack_errors_log (stack stk, long long errors);
	int __stack_dump       (stack stk);

	size_t __struct_Stack_size ();
	size_t __stack_elemsize    (stack stk);
	int    __stack_errcount    (stack stk);

#endif

#ifdef TYPE
#define CONCAT_HELP(name_1, name_2) name_1##_##name_2 
#define CONCAT(name_1, name_2) CONCAT_HELP(name_1, name_2)
#define OVERLOAD(name) CONCAT (TYPE, name)

stack OVERLOAD (stack_ctor) (size_t capacity)
{
	if (!capacity)
	{
		fprintf (stderr, "stack_ctor: bad arguments\n");
		return NULL;
	}

	stack stk = (stack) calloc (1, __struct_Stack_size () );	

	if (__stack_ctor  (stk, capacity, sizeof (TYPE) ) )
	{
		fprintf (stderr, "stack_ctor: can't create stack\n");
		free (stk);
		return NULL;
	}

	return stk;
}

void OVERLOAD (stack_dtor) (stack *stk)
{
	if ( !stk || !(*stk) )
	{
		fprintf (stderr, "stack_dtor: bad arguments\n");
		return;
	}

	__stack_dtor (stk);
}

void OVERLOAD (stack_push) (stack stk, TYPE value)
{
	char str[128] = "";
	sprintf (str, "stack_push (%p)", stk);

	if ( !stk)	
	{
		fprintf (stderr, "%s: bad arguments\n", str);
		return;
	}
	
	if (__stack_errcount (stk) )	
	{
		fprintf (stderr, "%s: stack has errors\n", str);
		return;
	}

	if (__stack_check (stk) )
	{
		fprintf (stderr, "%s: stack_check found %d errors\n", str,  __stack_errcount (stk) );

		return;
	}

	if (__stack_elemsize (stk) != sizeof (TYPE) )
	{
		fprintf (stderr, "%s: bad type of value\n", str);
		return;
	}	
	
	if (__stack_push (stk, &value) )	
		fprintf (stderr, "%s: can't push value in stack\n", str);
}

void OVERLOAD (stack_pop) (stack stk, TYPE *value)
{
	char str[128] = "";
	sprintf (str, "stack_pop (%p)", stk);

	if ( !stk)	
	{
		fprintf (stderr, "%s: bad arguments\n", str);
		return;
	}
	
	if (__stack_errcount (stk) )	
	{
		fprintf (stderr, "%s: stack has errors\n", str);
		return;
	}

	if (__stack_check (stk) )
	{
		fprintf (stderr, "%s: stack_check found %d errors\n", str,  __stack_errcount (stk) );

		return;
	}

	if (__stack_elemsize (stk) != sizeof (TYPE) )
	{
		fprintf (stderr, "%s: bad type of value\n", str);
		return;
	}	

	if (__stack_is_empty (stk) ) 
	{
		fprintf (stderr, "%s: stack is empty\n", str);
		return;
	}

	if (__stack_pop (stk, value) )
		fprintf (stderr, "%s: can't pop value from stack\n", str);
} 

#undef CONCAT
#undef CONCAT_HELP
#undef OVERLAD

#endif
