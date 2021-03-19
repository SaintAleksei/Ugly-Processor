#define TYPE int
#include "dynarr.h"
#undef TYPE

int main ()
{
	printf ("# Trying to create dynamic array of 10 integers...\n");

	int_arr iarr= {};

	if (arr_create (iarr, 10, sizeof (int) ) == ARR_ERROR)
	{
		printf ("# Failure! Try to find an error in the code\n");
		return -1;
	}

	printf ("# Success!\n");	

	printf ("# Trying to fill array by 10 numbers...\n");

	while (iarr.counter < 10)
	{
		printf ("#\t[%02lu] = %lu\n", iarr.counter, iarr.counter + 1);

		iarr.arr[iarr.counter] = iarr.counter + 1;
		iarr.counter++;
	}

	printf ("# Success!\n");

	printf ("# Trying to expand array...\n");

	if (arr_x2expand (iarr) == ARR_ERROR)
	{
		printf ("# Failure! Try to find an error in thr code\n");
		return -1;
	}

	printf ("# Success!\n");

	printf ("# Trying to add 10 numbers to the array...\n");

	while (iarr.counter < 20)
	{
		printf ("#\t[%02lu] = %lu\n", iarr.counter, iarr.counter + 1);

		iarr.arr[iarr.counter] = iarr.counter + 1;
		iarr.counter++;
	}
	
	printf ("# Success!\n");

	printf ("# Trying to destroy array...\n");

	if (arr_destroy (iarr) == ARR_ERROR)
	{
		printf ("# Failure! Try to find an error in thr code\n");
		return -1;
	}
		
	printf ("# Success!\n");

	return 0;
}
