#include "CPU.h"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}	

	FILE *stream = NULL;
		
	if (!(stream = fopen (argv[1], "r") ) )
	{
		fprintf (stderr, "%s: Error: Can't open file \"%s\"\n"
						 "Usage: %s <filename>\n", argv[0], argv[1], argv[0]);
		return 1;
	}

	cpu_t cpu = {};

	CPULoad (stream, &cpu);

	fclose (stream);

	CPURun (&cpu);

	return 0;
}
