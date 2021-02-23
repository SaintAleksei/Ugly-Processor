#ifndef STACK_H_INCLUDED

	#define STACK_H_INCLUDED 1

	#include <stdio.h>
	#include <stdlib.h>

	typedef struct Stack * stack;

	int  __stack_ctor      (stack stk, size_t capacity, size_t elemsize);
	int  __stack_dtor      (stack *stk);
	int  __stack_push      (stack stk, const void *value);
	int  __stack_pop       (stack stk, void *value);
	int  __stack_is_empty  (stack stk);
	int  __stack_check     (stack stk);
	int  __stack_check_log (stack stk);
	int  __stack_errors    (stack stk);
	size_t __struct_Stack_size ();
	size_t __stack_elemsize (stack stk);


#endif

#ifdef TYPE

#define CONCAT_HELP(name_1, name_2) name_1##_##name_2
#define CONCAT(name_1, name_2) CONCAT_HELP(name_1, name_2)
#define OVERLOAD(name) CONCAT (TYPE, name)

stack OVERLOAD (stack_ctor) (size_t capacity)
{
	if (!capacity)
	{
		fprintf (stderr, "stack_ctor: bad parameters\n");
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
		fprintf (stderr, "stack_dtor: bad parameters\n");
		return;
	}

	__stack_dtor (stk);
}

void OVERLOAD (stack_push) (stack stk, TYPE value)
{
	if ( !stk)	
	{
		fprintf (stderr, "stack_push: bad parameters\n");
		return;
	}
	
	if (__stack_elemsize (stk) != sizeof (TYPE) )
	{
		fprintf (stderr, "stack_push: bad type of value\n");
		return;
	}

	if (__stack_check (stk) )
	{
		fprintf (stderr, "stack_push: stack_check found %d erros in stack\n"
						 "look in the stack_check_log to get more info\n",  __stack_errors (stk) );

		exit (__stack_errors (stk) );
	}
	
	if (__stack_push (stk, &value) )	
		fprintf (stderr, "stack_push: can't push value in stack\n");
}

void OVERLOAD (stack_pop) (stack stk, TYPE *value)
{
	if ( !stk || !value )
	{
		fprintf (stderr, "stack_pop: bad parameters\n");
		return;
	}

	if (__stack_elemsize (stk) != sizeof (TYPE) )
	{
		fprintf (stderr, "stack_push: bad type of value\n");
		return;
	}

	if (__stack_is_empty (stk) ) 
	{
		fprintf (stderr, "stack_pop: stack is empty\n");
		return;
	}

	if (__stack_check (stk) )
	{
		fprintf (stderr, "stack_pop: stack_check found %d erros in stack\n"
						 "look in the stack_check_log to get more info\n",  __stack_errors (stk));
		
		exit (__stack_errors (stk) );
	}

	if (__stack_pop (stk, value) )
		fprintf (stderr, "stack_pop: can't pop value from stack\n");
} 

#undef CONCAT
#undef CONCAT_HELP
#undef OVERLAD

#endif
