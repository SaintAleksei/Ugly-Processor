#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../commands.h"

typedef struct label
{
	char name[64];
	cmd_t jmp_arg;
	cmd_t jmp_adr;
	char jmp_adr_flag;
	char jmp_arg_flag;
} label_t;

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <filename>\n", argv[0]);
		return -1;
	}	

	FILE *stream = NULL;
		
	if (!(stream = fopen (argv[1], "r") ) )
	{
		fprintf (stderr, "%s: Error: Can't open file \"%s\"\n"
						 "Usage: %s <filename>\n", argv[0], argv[1], argv[0]);
		return -1;
	}

	size_t labels_capacity = 1;
	size_t labels_count    = 0;
	label_t *labels        = calloc (labels_capacity, sizeof (label_t) );

	cmd_t instsize   = 2;
	cmd_t pc         = 0;
	cmd_t *inst      = calloc (instsize, sizeof (cmd_t) );

	int  line       = 0;
	char word[64]   = "";
	char error_flag = 0;

	while (fscanf (stream, "%s", word) == 1)
	{
		line++;
	
		if (strcmp (word, "push") == 0)
		{
			if (fscanf (stream, "%s", word) != 1)
			{
				error_flag = 1;
				break;
			}	

			if (isalpha (word[0]) )
			{
				inst[pc++] = CMD_PUSH_REG;

				if (strcmp (word, "r1") == 0)
					inst[pc++] = REG_R1;
				else if (strcmp (word, "r2") == 0)
					inst[pc++] = REG_R2;
				else if (strcmp (word, "r3") == 0)
					inst[pc++] = REG_R3;
				else if (strcmp (word, "r4") == 0)
					inst[pc++] = REG_R4;
				else
				{
					error_flag = 1;
					break;
				}
			}
			else if (isdigit (word[0]) || word[0] == '+' || word[0] == '-')
			{
				inst[pc++] = CMD_PUSH_CONST;
			
				if (sscanf (word, "%lg", (double *) (inst + pc) ) != 1)
				{
					error_flag = 1;
					break;
				}
				else
					pc++;
			}
			else 
			{
				error_flag = 1;
				break;
			}	
		}
		else if (strcmp (word, "pop") == 0)
		{
			if (fscanf (stream, "%s", word) != 1)
			{
				error_flag = 1;
				break;
			}	
		
			inst[pc++] = CMD_POP;

			if (strcmp (word, "r1") == 0)
				inst[pc++] = REG_R1;
			else if (strcmp (word, "r2") == 0)
				inst[pc++] = REG_R2;
			else if (strcmp (word, "r3") == 0)
				inst[pc++] = REG_R3;
			else if (strcmp (word, "r4") == 0)
				inst[pc++] = REG_R4;
			else
			{
				error_flag = 1;
				break;
			}
		}
		else if (strcmp (word, "add") == 0)
			inst[pc++] = CMD_ADD;
		else if (strcmp (word, "sub") == 0)
			inst[pc++] = CMD_SUB;
		else if (strcmp (word, "mul") == 0)
			inst[pc++] = CMD_MUL;
		else if (strcmp (word, "div") == 0)
			inst[pc++] = CMD_DIV;
		else if (strcmp (word, "fsqrt") == 0)
			inst[pc++] = CMD_FSQRT;
		else if (strcmp (word, "in") == 0)
			inst[pc++] = CMD_IN;
		else if (strcmp (word, "out") == 0)
			inst[pc++] = CMD_OUT;
		else if (strcmp (word, "hlt") == 0)
			inst[pc++] = CMD_HLT;
		else if (strcmp (word, "nop") == 0)
			inst[pc++] = CMD_NOP;
		else if (strcmp (word, "jmp") == 0)	
		{
			inst[pc++] = CMD_JMP;

			if (fscanf (stream, "%s", word) != 1)
			{
				error_flag = 1;
				break;
			}

			char found = 0;
			for (size_t i = 0; !found && i < labels_count; i++)
				if (strcmp (word, labels[i].name) == 0)
				{
					if (labels[i].jmp_adr_flag)
					{
						error_flag = 1;
						break;
					}

					found++;	
					labels[i].jmp_adr = pc;
					labels[i].jmp_adr_flag = 1;
				}

			if (!found)
			{
				strncpy (labels[labels_count].name, word, strlen (word) + 1);
				labels[labels_count].jmp_adr = pc;
				labels[labels_count].jmp_adr_flag = 1;
				labels[labels_count].jmp_arg_flag = 0;
				labels_count++;
			}
	
			pc++;
		}
		else if (word[strlen (word) - 1] == ':')	
		{
			word[strlen (word) - 1] = 0;

			char found = 0;
			for (size_t i = 0; !found && i < labels_count; i++)
				if (strcmp (word, labels[i].name) == 0)
				{
					if (labels[i].jmp_arg_flag)
					{
						error_flag = 1;
						break;
					}

					found++;	
					labels[i].jmp_arg = pc;
					labels[i].jmp_arg_flag = 1;
				}

			if (!found)
			{
				strncpy (labels[labels_count].name, word, strlen (word) + 1);
				labels[labels_count].jmp_arg = pc;
				labels[labels_count].jmp_adr_flag = 0;
				labels[labels_count].jmp_arg_flag = 1;
				labels_count++;
			}
		}
		else
		{
			error_flag = 1;
			break;
		}	

		if (pc >= instsize)
		{
			instsize *= 2;

			inst = realloc (inst, instsize * sizeof (cmd_t) );

			for (size_t i = pc; i < instsize; i++)
				inst[i] = 0;
		} 

		if (labels_count >= labels_capacity)
		{
			labels_capacity *= 2;
			
			labels = realloc (labels, labels_capacity * sizeof (label_t) );	

			for (size_t i = labels_count; i < labels_capacity; i++)
				for (size_t j = 0; j < sizeof (label_t); j++)
					*( (char *) (labels + i) + j) = 0;
		}
	}

	for (size_t i = 0; i < labels_count; i++)
	{
		if (!labels[i].jmp_arg_flag || !labels[i].jmp_adr_flag)
		{
			error_flag = 1;
			break;
		}

		inst[labels[i].jmp_adr] = labels[i].jmp_arg;
	}

	if (error_flag)
	{
		fprintf (stderr, "%s:%s:%d: can't make CPU instruction\n", argv[0], argv[1], line);
		return 1;
	}

	fclose (stream);

	char fname[64] = "";
	if (strchr (argv[1], '.') )
		*strchr (argv[1], '.') = 0;
	sprintf (fname, "%s.code", argv[1]);

	stream = fopen (fname, "w");

	fwrite (inst, sizeof (cmd_t), pc, stream);

	fclose (stream);

	free (inst);
	free (labels);
	return 0;
}
