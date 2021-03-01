#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../commands.h"

#define MAX_NAME_SIZE 64

typedef struct label
{
	char name[MAX_NAME_SIZE];
	cmd_t adres;
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

	size_t labels_capacity = 1024;
	size_t labels_count    = 0;
	label_t *labels        = calloc (labels_capacity, 1);

	size_t fixup_capacity  = 1024;
	size_t fixup_count     = 0;
	label_t *fixup         = calloc (fixup_capacity, 1);

	size_t inst_capacity   = 1024;
	size_t pc              = 0;
	cmd_t *inst            = calloc (inst_capacity, 1);

	int  line                  = 0;
	char word[MAX_NAME_SIZE]   = "";
	char error_flag            = 0;

	while (fscanf (stream, "%s", word) == 1)
	{
		char jmp_flag = 0;

		line++;
	
		if (strcmp (word, "push") == 0)
		{
			cmd_t reg = 0;
			fscanf (stream, "%s", word);

			if (sscanf (word, "r%llu", &reg) == 1)
			{
				if (reg >= INT_REGISTERS)
				{
					error_flag = 1;
					break;
				}

				inst[pc++] = CMD_PUSH_REG;
				inst[pc++] = reg;
			}	
			else if (sscanf (word, "%lld", (int_t *) &reg) == 1)
			{
				inst[pc++] = CMD_PUSH_CONST;
				inst[pc++] = reg;	
			}
			else 
			{
				error_flag = 1;
				break;
			}	
		}
		else if (strcmp (word, "pushr") == 0)
		{
			cmd_t reg = 0;
			fscanf (stream, "%s", word);

			if (sscanf (word, "rr%llu", &reg) == 1)
			{
				if (reg >= REAL_REGISTERS)
				{
					error_flag = 1;
					break;
				}

				inst[pc++] = CMD_PUSHR_REG;
				inst[pc++] = reg;
			}	
			else if (sscanf (word, "%lg", (real_t *) &reg) == 1)
			{
				inst[pc++] = CMD_PUSHR_CONST;
				inst[pc++] = reg;	
			}
			else 
			{
				error_flag = 1;
				break;
			}	
		}
		else if (strcmp (word, "pop") == 0)
		{
			cmd_t reg = 0;
			fscanf (stream, "%s", word);

			if (sscanf (word, "r%llu", &reg) == 1)
			{
				if (reg > INT_REGISTERS)
				{
					error_flag = 1;
					break;
				}

				inst[pc++] = CMD_POP;
				inst[pc++] = reg;
			}	
			else
			{
				error_flag = 1;
				break;
			}
		}
		else if (strcmp (word, "popr") == 0)
		{
			cmd_t reg = 0;
			fscanf (stream, "%s", word);

			if (sscanf (word, "rr%llu", &reg) == 1)
			{
				if (reg > REAL_REGISTERS)
				{
					error_flag = 1;
					break;
				}

				inst[pc++] = CMD_POPR;
				inst[pc++] = reg;
			}	
			else
			{
				error_flag = 1;
				break;
			}
		}
		else if (strcmp (word, "add") == 0)
			inst[pc++] = CMD_ADD;
		else if (strcmp (word, "addr") == 0)
			inst[pc++] = CMD_ADDR;
		else if (strcmp (word, "sub") == 0)
			inst[pc++] = CMD_SUB;
		else if (strcmp (word, "subr") == 0)
			inst[pc++] = CMD_SUBR;
		else if (strcmp (word, "mul") == 0)
			inst[pc++] = CMD_MUL;
		else if (strcmp (word, "mulr") == 0)
			inst[pc++] = CMD_MULR;
		else if (strcmp (word, "div") == 0)
			inst[pc++] = CMD_DIV;
		else if (strcmp (word, "divr") == 0)
			inst[pc++] = CMD_DIVR;
		else if (strcmp (word, "fsqrt") == 0)
			inst[pc++] = CMD_FSQRT;
		else if (strcmp (word, "rti") == 0)
			inst[pc++] = CMD_RTI;
		else if (strcmp (word, "itr") == 0)
			inst[pc++] = CMD_ITR;
		else if (strcmp (word, "in") == 0)
			inst[pc++] = CMD_IN;
		else if (strcmp (word, "inr") == 0)
			inst[pc++] = CMD_INR;
		else if (strcmp (word, "get") == 0)
			inst[pc++] = CMD_GET;
		else if (strcmp (word, "out") == 0)
			inst[pc++] = CMD_OUT;
		else if (strcmp (word, "outr") == 0)
			inst[pc++] = CMD_OUTR;
		else if (strcmp (word, "put") == 0)
			inst[pc++] = CMD_PUT;
		else if (strcmp (word, "hlt") == 0)
			inst[pc++] = CMD_HLT;
		else if (strcmp (word, "nop") == 0)
			inst[pc++] = CMD_NOP;
		else if (strcmp (word, "ret") == 0)
			inst[pc++] = CMD_RET;
		else if (strcmp (word, "call") == 0)
		{
			inst[pc++] = CMD_CALL;
			jmp_flag = 1;
		}
		else if (strcmp (word, "jmp") == 0)	
		{
			inst[pc++] = CMD_JMP;
			jmp_flag = 1;
		}
		else if (strcmp (word, "je") == 0 )
		{
			inst[pc++] = CMD_JE;	
			jmp_flag = 1;
		}
		else if (strcmp (word, "jne") == 0)
		{
			inst[pc++] = CMD_JNE;
			jmp_flag = 1;
		}
		else if (strcmp (word, "jb") == 0)
		{
			inst[pc++] = CMD_JB;
			jmp_flag = 1;
		}
		else if (strcmp (word, "jbe") == 0)
		{
			inst[pc++] = CMD_JBE;
			jmp_flag = 1;
		}
		else if (strcmp (word, "ja") == 0)
		{
			inst[pc++] = CMD_JA;
			jmp_flag = 1;
		}
		else if (strcmp (word, "jae") == 0)
		{
			inst[pc++] = CMD_JAE;
			jmp_flag = 1;
		}
		else if (word[strlen (word) - 1] == ':')	
		{
			word[strlen (word) - 1] = 0;
			
			for (size_t i = 0; !error_flag && i < labels_count; i++)
				if (strcmp (word, labels[i].name) == 0)
					error_flag = 1;

			if (error_flag)
				break;
			else
			{
				labels[labels_count].adres = pc;
				strncpy (labels[labels_count].name, word, MAX_NAME_SIZE);
				labels_count++;
			}	
		}
		else
		{
			error_flag = 1;
			break;
		}	

		if (jmp_flag)
		{
			if (fscanf (stream, "%s", word) != 1)
			{
				error_flag = 1;	
				break;
			}

			char found = 0;
			for (size_t i = 0; !found && i < labels_count; i++)
				if (strcmp (word, labels[i].name) == 0)
				{
					found = 1;
					inst[pc] = labels[i].adres;
				}

			if (!found) 
			{
				fixup[fixup_count].adres = pc;
				strncpy (fixup[fixup_count].name, word, MAX_NAME_SIZE);
				fixup_count++;
			}

			pc++;
		}

		if ( (pc + 2) * sizeof (cmd_t) >= inst_capacity)
		{
			inst_capacity *= 2;

			inst = realloc (inst, inst_capacity);

			for (size_t i = pc * sizeof (cmd_t); i < inst_capacity; i++)
				*( (char *) inst + i) = 0;
		} 

		if ( (labels_count + 1) * sizeof (label_t) >= labels_capacity)
		{
			labels_capacity *= 2;
			
			labels = realloc (labels, labels_capacity);	

			for (size_t i = labels_count * sizeof (label_t); i < labels_capacity; i++)
				*( (char *) labels + i) = 0;
		}

		if ( (fixup_count + 1) * sizeof (label_t)  >= fixup_capacity)
		{
			fixup_capacity *= 2;
			
			fixup = realloc (fixup, fixup_capacity * sizeof (label_t) );	

			for (size_t i = fixup_count * sizeof (label_t); i < fixup_capacity; i++)
				*( (char *) fixup + i) = 0;
		}
	}

	for (size_t i = 0; i < fixup_count; i++)
	{
		char found = 0;
		for (size_t j = 0; !found && j < labels_count; j++)
			if (strcmp (labels[j].name, fixup[i].name) == 0)
			{
				found = 1;
				inst[fixup[i].adres] = labels[j].adres;
			}

		if (!found)
		{
			error_flag = 1;
			break;
		}
	}
		
	if (error_flag)
	{
		fprintf (stderr, "%s: %s (%d): Illegal instruction\n", argv[0], argv[1], line);
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
	free (fixup);
	return 0;
}
