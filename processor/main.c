#include "processor.h"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <name of file>\n", argv[0]);

		return 1;
	}	

	proc_t proc = {};

	int ret = proc_load (&proc, argv[1]);
	
	if (ret)
	{
		fprintf (stderr, "%s: [ERROR  ]: Can't load processor from file '%s'\n", argv[0], argv[1]);

		return 1;
	}

	printf ("%s: [SUCCESS]: Processor loaded successful!\n", argv[0]);

	ret = proc_run (&proc);

	if (ret == PROC_HALTED)
		printf ("%s: [SUCCESS]: Processor halted successful!\n", argv[0]);
	else
		printf ("%s: [ERROR  ]: Processor halted with bad status!\n", argv[0]);
	
	proc_destroy (&proc);

	return 0;
}
