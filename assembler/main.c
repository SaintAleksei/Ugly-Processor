#include "assembler.h"

#define TYPE cmd_t 
#include "../dynarr/dynarr.h"
#undef TYPE

#define TYPE label_t
#include "../dynarr/dynarr.h"
#undef TYPE

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf (stderr, "%s: Usage: %s <name of file>\n", argv[0], argv[0]);
		return -1;
	}	

	FILE *stream = fopen (argv[1], "r");

	if (!stream)
	{
		fprintf (stderr, "%s: Can't open file '%s': ", argv[0], argv[1]);	
		return -1;
	}
	
	cmd_t_arr inst     = {};
	label_t_arr labels = {};
	label_t_arr fixups = {};

	arr_create (inst,   sizeof (cmd_t)   );
	arr_create (labels, sizeof (label_t) );
	arr_create (fixups, sizeof (label_t) );

	char   buffer[MAX_BUFFER_SIZE] = "";
	char   label[MAX_LABEL_SIZE]   = "";
	size_t line                    = 0;
	cmd_t  cmd                     = 0;
	char   iserror             	   = 0;
	char   islabel                 = 0;

	while ( !iserror && fgets (buffer, MAX_BUFFER_SIZE, stream) )
	{
		line++;
		*label  = 0;
		islabel = 0;

		if (get_label (label, buffer) == 0)
		{
			islabel = 1;

			if (find_label (labels.arr, labels.counter, label) == CMD_ERROR)	
			{
				labels.arr[labels.counter].addr = inst.counter;		
				strncpy (labels.arr[labels.counter].name, label, MAX_LABEL_SIZE);
				labels.counter++;	
			}
			else
				iserror = 1;
		}
			
		cmd = get_cmd (buffer);	

		if (cmd != CMD_ERROR)
		{
			inst.arr[inst.counter] = cmd;
			inst.counter++;
		}
		else if (!islabel)
			iserror = 1;

		if (!islabel && !iserror && cmd & MASK_PUSH)
		{
			cmd = get_push_arg (buffer, cmd);

			if (cmd != CMD_ERROR)
			{
				inst.arr[inst.counter] = cmd;
				inst.counter++;
			}
			else
				iserror = 1;
		}
		else if (!islabel && !iserror && cmd & MASK_POP)
		{
			cmd = get_pop_arg (buffer, cmd);

			if (cmd != CMD_ERROR)
			{
				inst.arr[inst.counter] = cmd;
				inst.counter++;
			}
			else
				iserror = 1;
		}
		else if (!islabel && !iserror && cmd & MASK_JMP)
		{
			if (get_jmp_arg (label, buffer) == 0)	
			{
				cmd = find_label (labels.arr, labels.counter, label);

				if (cmd != CMD_ERROR)
					inst.arr[inst.counter] = cmd;
				else
				{
					inst.arr[inst.counter]          = CMD_ERROR;
					strncpy (fixups.arr[fixups.counter].name, label, MAX_LABEL_SIZE);
					fixups.arr[fixups.counter].addr = inst.counter;
					fixups.arr[fixups.counter].line = line;
					fixups.counter++;
				}

				inst.counter++;
			}
			else
				iserror = 1;
		}

		if ( (inst.counter + 1) * inst.elemsize >= inst.capacity)
			arr_x2expand (inst);

		if ( (labels.counter + 1) * labels.elemsize >= labels.capacity)
			arr_x2expand (labels);

		if ( (fixups.counter + 1) * labels.counter >= fixups.capacity)
			arr_x2expand (fixups);
	}

	if (iserror)
	{
		buffer[strlen(buffer) - 1] = 0;
		
		size_t i = 0;
		while (isspace (buffer[i]) )
			i++;

		fprintf (stderr, "@ %s:[ERROR]:%s:%lu: Illegal instruction: '%s'\n", argv[0], argv[1], line, buffer + i);

		fclose (stream);
		arr_destroy (inst);
		arr_destroy (labels);
		arr_destroy (fixups);

		return 1;
	}

	for (size_t i = 0; i < fixups.counter; i++)
	{
		cmd = find_label (labels.arr, labels.counter, fixups.arr[i].name);	

		if (cmd != CMD_ERROR)
			inst.arr[fixups.arr[i].addr] = cmd;
		else
		{
			fprintf (stderr, "@ %s:[ERROR]:%s:%lu: Unknown label '%s'\n", argv[0], argv[1], fixups.arr[i].line, fixups.arr[i].name);

			fclose (stream);
			arr_destroy (inst);
			arr_destroy (labels);
			arr_destroy (fixups);

			return 2;
		}
	}

	fclose (stream);

	strncpy (buffer, argv[1], MAX_BUFFER_SIZE);
	change_format (buffer, ".bin");
	
	stream = fopen (buffer, "wb");
	
	int regs = (REGISTERS > 99) ? 99 : REGISTERS;
	fprintf (stream, "proc-r%d", regs);

	if (!stream)
		fprintf (stderr, "%s: Can't open file '%s'\n", argv[0], argv[1]);
	else
		fwrite (inst.arr, sizeof (inst.arr[0]), inst.counter, stream);

	fclose (stream);
	arr_destroy (inst);	
	arr_destroy (labels);
	arr_destroy (fixups);

	return 0;
}
