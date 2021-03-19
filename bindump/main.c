#include "bindump.h"

int main (int argc, char *argv[])
{	
	if (argc < 2)
	{
		fprintf (stderr, "%s: Usage: %s <name of file>\n", argv[0], argv[0]);

		return -1;
	}	

	printf ("----------$$$ bdump $$$----------\n");

	size_t fsize = 0;
	cmd_t *data  = super_fread (argv[1], sizeof (cmd_t), &fsize);

	for (size_t i = 0; i < fsize; i++)
	{
		cmd_t cmd = data[i];

		printf ("%06lx: 0x%016llx", i, cmd);

		char command[MAX_NAME_SIZE] = "";

		if (decode_cmd (command, cmd) )
			printf (": %s", command);

		printf ("\n");
	}

	printf ("----------$$$ bdump $$$----------\n");

	free (data);

	return 0;
}
