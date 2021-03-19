#ifndef DYNARR_H_INCLUDED
#define DYNARR_H_INCLUDED

#define ARR_ERROR -1

#include <stdio.h>
#include <stdlib.h>		

typedef struct array
{
	void *arr;
	size_t counter;
	size_t capacity;
	size_t elemsize;
} arr_t;

int __arr_create   (arr_t *ptr, size_t elemsize, size_t line, const char *fname, const char *vname);
int __arr_destroy  (arr_t *ptr, size_t line, const char *fname, const char *vname);
int __arr_x2expand (arr_t *ptr, size_t line, const char *fname, const char *vname);

#define arr_create(A, B) 										\
	( (sizeof (A) == sizeof (arr_t) ) ? __arr_create ( (arr_t*) &A, B, __LINE__, __FILE__, #A) : __arr_create (NULL, B, __LINE__, __FILE__, #A) )

#define arr_destroy(A) 												\
	( (sizeof (A) == sizeof (arr_t) ) ? __arr_destroy ( (arr_t*) &A, __LINE__, __FILE__, #A) : __arr_destroy (NULL, __LINE__, __FILE__, #A) )

#define arr_x2expand(A) 											\
	( (sizeof (A) == sizeof (arr_t) ) ? __arr_x2expand ( (arr_t*) &A, __LINE__, __FILE__, #A) : __arr_x2expand (NULL, __LINE__, __FILE__, #A) )

#endif

#ifdef TYPE

#define CONCAT_HELP(name_1, name_2) name_1##_##name_2
#define CONCAT(name_1, name_2) CONCAT_HELP (name_1, name_2)
#define OVERLOAD(name) CONCAT (TYPE, name)

typedef struct OVERLOAD (array)
{
	TYPE* arr;
	size_t counter;
	size_t capacity;
	size_t elemsize;
} OVERLOAD (arr);

#undef OVERLOAD
#undef CONCAT
#undef CONCAT_HELP

#endif 
