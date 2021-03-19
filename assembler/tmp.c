#include <stdio.h>

int main ()
{
	char str[64] = "";
	char buf[64] = "";
	int  a       = 0;

	a = scanf ("%s:", buf);

	printf ("a = %d, buf = %s\n", a, buf);	

	return 0;
}
