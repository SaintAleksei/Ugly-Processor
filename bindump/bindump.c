#include "bindump.h"
#include "../commands.h"

char *decode_cmd (char *dest, cmd_t cmd)
{
	if (!dest)
		return NULL;

	switch (cmd)
	{
		case CMD_PUSH_C:
			strncpy (dest, "push <const>", MAX_NAME_SIZE);
			break;

		case CMD_PUSH_R:
			strncpy (dest, "push <reg>", MAX_NAME_SIZE);
			break;

		case CMD_PUSH_RAM_R:
			strncpy (dest, "push (<reg>)", MAX_NAME_SIZE);
			break;

		case CMD_PUSH_RAM_C:
			strncpy (dest, "push (<const>)", MAX_NAME_SIZE);
			break;

		case CMD_PUSHR_C:
			strncpy (dest, "pushr <const>", MAX_NAME_SIZE);
			break;

		case CMD_PUSHR_R:
			strncpy (dest, "pushr <reg>", MAX_NAME_SIZE);
			break;

		case CMD_PUSHR_RAM_R:
			strncpy (dest, "pushr (<reg>)", MAX_NAME_SIZE);
			break;

		case CMD_PUSHR_RAM_C:
			strncpy (dest, "pushr (<const>)", MAX_NAME_SIZE);
			break;

		case CMD_POP:
			strncpy (dest, "pop <reg>", MAX_NAME_SIZE);
			break;

		case CMD_POP_RAM_C:
			strncpy (dest, "pop (<const>)", MAX_NAME_SIZE);
			break;

		case CMD_POP_RAM_R:
			strncpy (dest, "pop (<reg>)", MAX_NAME_SIZE);
			break;

		case CMD_POPR:
			strncpy (dest, "popr <reg>", MAX_NAME_SIZE);
			break;

		case CMD_POPR_RAM_C:
			strncpy (dest, "popr (<const>)", MAX_NAME_SIZE);
			break;

		case CMD_POPR_RAM_R:
			strncpy (dest, "popr (<reg>)", MAX_NAME_SIZE);
			break;

		case CMD_JMP:
			strncpy (dest, "jmp <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JE:
			strncpy (dest, "je <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JER:
			strncpy (dest, "jer <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JNE:
			strncpy (dest, "jne <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JNER:
			strncpy (dest, "jner <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JB:
			strncpy (dest, "jb <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JBR:
			strncpy (dest, "jbr <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JBE:
			strncpy (dest, "jbe <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JBER:
			strncpy (dest, "jber <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JA:
			strncpy (dest, "ja <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JAR:
			strncpy (dest, "jar <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JAE:
			strncpy (dest, "jae <addr>", MAX_NAME_SIZE);
			break;

		case CMD_JAER:
			strncpy (dest, "jaer <addr>", MAX_NAME_SIZE);
			break;

		case CMD_HLT:
			strncpy (dest, "hlt", MAX_NAME_SIZE);
			break;

		case CMD_ADD:
			strncpy (dest, "add", MAX_NAME_SIZE);
			break;

		case CMD_ADDR:
			strncpy (dest, "addr", MAX_NAME_SIZE);
			break;

		case CMD_SUB:
			strncpy (dest, "sub", MAX_NAME_SIZE);
			break;

		case CMD_SUBR:
			strncpy (dest, "subr", MAX_NAME_SIZE);
			break;

		case CMD_MUL:
			strncpy (dest, "mul", MAX_NAME_SIZE);
			break;

		case CMD_MULR:	
			strncpy (dest, "mulr", MAX_NAME_SIZE);
			break;

		case CMD_DIV:
			strncpy (dest, "div", MAX_NAME_SIZE);
			break;

		case CMD_DIVR:
			strncpy (dest, "divr", MAX_NAME_SIZE);
			break;

		case CMD_FSQRT:
			strncpy (dest, "fsqrt", MAX_NAME_SIZE);
			break;

		case CMD_RTI:
			strncpy (dest, "rti", MAX_NAME_SIZE);
			break;

		case CMD_ITR:	
			strncpy (dest, "itr", MAX_NAME_SIZE);
			break;

		case CMD_IN:
			strncpy (dest, "in", MAX_NAME_SIZE);
			break;

		case CMD_INR:
			strncpy (dest, "inr", MAX_NAME_SIZE);
			break;

		case CMD_OUT:
			strncpy (dest, "out", MAX_NAME_SIZE);
			break;

		case CMD_OUTR:
			strncpy (dest, "outr", MAX_NAME_SIZE);
			break;

		case CMD_RET:
			strncpy (dest, "ret", MAX_NAME_SIZE);
			break;

		case CMD_CALL:
			strncpy (dest, "call <addr>", MAX_NAME_SIZE);
			break;

		case CMD_NOP:	
			strncpy (dest, "nop", MAX_NAME_SIZE);
			break;

		default:
			return NULL;
	}

	return dest;
}
