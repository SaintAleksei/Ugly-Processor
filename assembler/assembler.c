#include "assembler.h"

#define MAX_CMD_SIZE 128

cmd_t jmp (const char *str, cmd_t cmd);

cmd_t get_cmd (const char *str)
{
	if (!str)
		return CMD_ERROR;

	cmd_t cmd               = 0;
	cmd_t cmd_tmp           = 0;
	char word[MAX_CMD_SIZE] = "";
	
	if (sscanf (str, "%s", word) != 1)
		return CMD_ERROR;

	size_t len = strlen (word);

	if (word[len-1] == 'r') 
	{
		cmd |= MASK_REAL;
		word[len-1] = 0;
		len--;
	}

	if ( (cmd_tmp = jmp (word, cmd) ) != CMD_ERROR)
		return cmd_tmp;

	if (strcmp (word, "push" ) == 0)
	{
		if (sscanf (str, "%s%s", word, word) != 2)
			cmd = CMD_ERROR;
		else if (isdigit (word[0]) || word[0] == '+' || word[0] == '-') 
			cmd |= CMD_PUSH_C;
		else if (word[0] == 'r')
			cmd |= CMD_PUSH_R;
		else if (word[0] == '(' && isdigit (word[1]) )
			cmd |= CMD_PUSH_RAM_C;
		else if (word[0] == '(' && word[1] == 'r')
			cmd |= CMD_PUSH_RAM_R;
		else
			cmd = CMD_ERROR;
	}
	else if (strcmp (word, "pop"  ) == 0)
	{
		if (sscanf (str, "%s%s", word, word) != 2)
			cmd = CMD_ERROR;
		else if (word[0] == 'r')
			cmd |= CMD_POP;
		else if (word[0] == '(' && isdigit (word[1]) )
			cmd |= CMD_POP_RAM_C;
		else if (word[0] == '(' && word[1] == 'r')
			cmd |= CMD_POP_RAM_R;
		else
			cmd = CMD_ERROR;
	}
	else if (strcmp (word, "add"  ) == 0)
		cmd |= CMD_ADD;
	else if (strcmp (word, "sub"  ) == 0)
		cmd |= CMD_SUB;
	else if (strcmp (word, "mul"  ) == 0)
		cmd |= CMD_MUL;
	else if (strcmp (word, "div"  ) == 0)
		cmd |= CMD_DIV;
	else if (strcmp (word, "fsqrt") == 0)
		cmd =  CMD_FSQRT;
	else if (strcmp (word, "rti"  ) == 0)
		cmd =  CMD_RTI;
	else if (strcmp (word, "itr"  ) == 0)	
		cmd =  CMD_ITR;
	else if (strcmp (word, "in"   ) == 0)
		cmd |= CMD_IN;
	/*else if (strcmp (word, "get"  ) == 0)	
		cmd =  CMD_GET; */
	else if (strcmp (word, "out"  ) == 0)
		cmd |= CMD_OUT;
	/*else if (strcmp (word, "put"  ) == 0)
		cmd =  CMD_PUT; */
	else if (strcmp (word, "call" ) == 0)
		cmd =  CMD_CALL;
	else if (strcmp (word, "ret"  ) == 0)
		cmd =  CMD_RET;
	else if (strcmp (word, "nop"  ) == 0)
		cmd =  CMD_NOP;
	else if (strcmp (word, "hlt"  ) == 0)
		cmd =  CMD_HLT;
	else
		cmd = CMD_ERROR;
	
	return cmd;
}

cmd_t get_push_arg (const char *str, cmd_t cmd)
{
	if (!str || cmd == CMD_ERROR || !(cmd & MASK_PUSH) )
		return CMD_ERROR;

	char word[MAX_CMD_SIZE] = "";
	
	switch (cmd)
	{
		case CMD_PUSH_C:
			if (sscanf (str, "%s %lld", word, (int_t *) &cmd) != 2)
				return CMD_ERROR;
			break;

		case CMD_PUSH_R:
			if (sscanf (str, "%s r%llu", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		case CMD_PUSHR_C:
			if (sscanf (str, "%s %lg", word, (real_t *) &cmd) != 2)
				return CMD_ERROR;
			break;

		case CMD_PUSHR_R:
			if (sscanf (str, "%s rr%llu", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		case CMD_PUSH_RAM_R:
		case CMD_PUSHR_RAM_R:
			if (sscanf (str, "%s (r%llu)", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		case CMD_PUSH_RAM_C:
		case CMD_PUSHR_RAM_C:
			if (sscanf (str, "%s (%llu)", word, &cmd) != 2)
				return CMD_ERROR;
			break;

		default:
			return CMD_ERROR;
	}

	return cmd;
}

cmd_t get_pop_arg (const char *str, cmd_t cmd)
{
	if (!str || cmd == CMD_ERROR || !(cmd & MASK_POP) )
		return CMD_ERROR;

	char word[MAX_CMD_SIZE] = "";

	switch (cmd)
	{
		case CMD_POP:
			if (sscanf (str, "%s r%llu", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		case CMD_POPR:
			if (sscanf (str, "%s rr%llu", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		case CMD_POP_RAM_C:
		case CMD_POPR_RAM_C:
			if (sscanf (str, "%s (%llu)", word, &cmd) != 2)
				return CMD_ERROR;
			break;

		case CMD_POP_RAM_R:
		case CMD_POPR_RAM_R:
			if (sscanf (str, "%s (r%llu)", word, &cmd) != 2 || cmd >= REGISTERS)
				return CMD_ERROR;
			break;

		default:
			return CMD_ERROR;
	}

	return cmd;
}

int get_jmp_arg (char *dest, const char *src)
{
	if (!dest || !src)
		return -1;

	if (sscanf (src, "%s %s", dest, dest) != 2)
		return 1;
	else
		return 0;
}

int get_label (char *dest, const char *src)
{
	if (!dest || !src)
		return -1;

	if (sscanf (src, "%s", dest) != 1)
		return 1;

	size_t len = strlen (dest);

	if (dest[len-1] != ':')
		return 2;
	else
		dest[len-1] = 0;
	
	return 0;
}

cmd_t find_label (label_t *labels, size_t labels_count, const char *name)
{
	if (!labels || !labels_count || !name)
		return CMD_ERROR;

	for (size_t i = 0; i < labels_count; i++)
		if (strcmp (labels[i].name, name) == 0)
			return labels[i].addr;

	return CMD_ERROR;
}

cmd_t jmp (const char *word, cmd_t cmd)
{
	if (!word || cmd == CMD_ERROR)
		return CMD_ERROR;

	if (word[0] == 'j')
	{
		switch (word[1])
		{
			case 'm':
				if (word[2] == 'p' && word[3] == 0)
					cmd = CMD_JMP;
				break;

			case 'e':
				if (word[2] == 0)
					cmd |= CMD_JE;
				break;

			case 'n':
				if (word[2] == 'e' && word[3] == 0)
					cmd |= CMD_JNE;
				break;

			case 'a':
				if (word[2] == 0)
					cmd |= CMD_JA;
				else if (word[2] == 'e' && word[3] == 0)
					cmd |= CMD_JAE;
				break;

			case 'b':	
				if (word[2] == 0)
					cmd |= CMD_JB;
				else if (word[2] == 'e' && word[3] == 0)
					cmd |= CMD_JBE;
				break;

			default:
				return CMD_ERROR;
		}

		if (! (cmd & MASK_JMP) ) 
			return CMD_ERROR;	
	}
	else
		return CMD_ERROR;

	return cmd;
}
