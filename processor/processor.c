#include "processor.h"

#define TYPE int_t
#include "../stack/stack.h"
#undef TYPE

#define TYPE real_t
#include "../stack/stack.h"
#undef TYPE

#define TYPE cmd_t
#include "../stack/stack.h"
#undef TYPE

#include "../ram/ram.h"

int proc_load (proc_t *proc, const char *fname)
{
	if (!proc || proc->status != PROC_DESTROYED || !fname)
		return __LINE__;

	size_t buffer_size = 0;
	cmd_t *buffer      = super_fread (fname, sizeof (cmd_t), &buffer_size);

	int regs = 0;
	if (sscanf ( (char *) buffer, "proc-r%d", &regs) != 1 || regs > REGISTERS)
	{
		free (buffer);

		return __LINE__;
	}

	proc->ram = calloc (1, sizeof (ram_t) );

	if (ram_create (proc->ram, buffer_size - 1, DATA_SIZE) )
	{
		free (buffer);
		free (proc->ram);

		return __LINE__;
	}

	if (ram_load (proc->ram, buffer + 1) )
	{
		free (buffer);
		free (proc->ram);

		return __LINE__;
	}

	free (buffer);

	proc->ip = 0;
		
	for (size_t i = 0; i < REGISTERS; i++)
	{
		proc->int_reg [i] = 0;
		proc->real_reg[i] = 0;
	}

	proc->int_stk  = int_t_stack_ctor   (1);
	proc->real_stk = real_t_stack_ctor  (1);
	proc->ret_stk  = cmd_t_stack_ctor (1);

	proc->status   = PROC_HALTED;

	return 0;	
}

int proc_destroy (proc_t *proc)
{
	if (!proc || !(proc->ram) || proc->status == PROC_DESTROYED)
		return __LINE__;

	real_t_stack_dtor (&(proc->real_stk) );
	int_t_stack_dtor  (&(proc->int_stk)  );
	cmd_t_stack_dtor  (&(proc->ret_stk)  );
	
	ram_destroy (proc->ram);
	free (proc->ram);

	memset (proc, 0, sizeof (proc_t) );

	proc->status = PROC_DESTROYED;

	return 0;
}

int proc_run (proc_t *proc)
{
	if (!proc || !proc->ram || proc->status != PROC_HALTED || proc->ram->status != RAM_OK)
		return __LINE__;	

	proc->status = PROC_RUNNING;

	while (proc->status == PROC_RUNNING && proc->ram->status == RAM_OK)
	{
		cmd_t cmd   = 0;
		char dojump = 1;

		ram_read (proc->ram, proc->ip, &cmd);

		if (cmd & MASK_ALU)
		{
			if (cmd & MASK_REAL)
				real_alu (proc, cmd, &dojump);
			else
				int_alu (proc, cmd, &dojump);
		}
		else if (cmd & MASK_PUSH)
			push (proc, cmd);
		else if (cmd & MASK_POP)
			pop  (proc, cmd);
		else if (!(cmd & MASK_JMP) )
		{
			real_t real_a = 0;
			int_t  int_a  = 0;
			cmd_t  cmd_a  = 0;

			switch (cmd)
			{
				case CMD_RTI:
					real_t_stack_pop (proc->real_stk, &real_a);
					int_a = (int_t) real_a;
					int_t_stack_push (proc->int_stk, int_a);
					break;

				case CMD_ITR:
					int_t_stack_pop (proc->int_stk, &int_a);
					real_a = (real_t) int_a;
					real_t_stack_push (proc->real_stk, real_a);
					break;

				case CMD_IN:
					printf ("#> ");
					scanf ("%lld", &int_a);
					int_t_stack_push (proc->int_stk, int_a);
					break;

				case CMD_INR:
					printf ("#> ");
					scanf ("%lg", &real_a);
					real_t_stack_push (proc->int_stk, real_a);
					break;

				case CMD_OUT:
					int_t_stack_pop (proc->int_stk, &int_a);
					printf ("#> %lld\n", int_a);
					int_t_stack_push (proc->int_stk, int_a);
					break;

				case CMD_OUTR:
					real_t_stack_pop (proc->real_stk, &real_a);
					printf ("#> %lg\n", real_a);
					real_t_stack_push (proc->real_stk, real_a);
					break;

				case CMD_RET:
					cmd_t_stack_pop (proc->ret_stk, &cmd_a);
					proc->ip = cmd_a;
					break;

				case CMD_NOP:
					break;

				case CMD_HLT:
					proc->status = PROC_HALTED;
					break;

				default:
					proc->status = PROC_ERROR;
			}

			if (proc->status == PROC_RUNNING && cmd != CMD_RET)
				proc->ip++;	
		}

		if (cmd & MASK_JMP)
			jmp  (proc, cmd, dojump);

		ram_check (proc->ram);
	}

	if (proc->ram->status != RAM_OK)
	{
		ram_error (proc->ram);
	
		FILE *log = fopen ("ram.log", "w");

		ram_log (proc->ram, log);

		fclose (log);

		log = fopen ("ram.dump", "w");

		ram_dump (proc->ram, log);

		fclose (log);

		proc->status = PROC_ERROR;
	}

	return proc->status;
}

int real_alu (proc_t *proc, cmd_t cmd, char *dojump)
{
	if (!proc || proc->status != PROC_RUNNING || !dojump)
		return __LINE__;

	real_t b   = 0;
	real_t a   = 0;
	*dojump    = 0;

	if (cmd == CMD_FSQRT)
	{
		real_t_stack_pop  (proc->real_stk, &a);
		real_t_stack_push (proc->real_stk, sqrt (a) );

		return cmd;
	}

	real_t_stack_pop  (proc->real_stk, &a);
	real_t_stack_pop  (proc->real_stk, &b);

	switch (cmd)
	{
		case CMD_JER:
			*dojump = ( (b == a) ? 1 : 0);
			break;

		case CMD_JNER:
			*dojump = ( (b != a) ? 1 : 0);
			break;

		case CMD_JBR:
			*dojump = ( (b < a) ? 1 : 0);
			break;

		case CMD_JBER:
			*dojump = ( (b <= a) ? 1 : 0);
			break;

		case CMD_JAR:
			*dojump = ( (b > a) ? 1 : 0);
			break;

		case CMD_JAER:
			*dojump = ( (b >= a) ? 1 : 0);
			break;

		case CMD_ADDR:
			real_t_stack_push (proc->real_stk, a + b);
			break;

		case CMD_SUBR: 
			real_t_stack_push (proc->real_stk, b - a);
			break;

		case CMD_MULR:
			real_t_stack_push (proc->real_stk, b * a);
			break;

		case CMD_DIVR:
			real_t_stack_push (proc->real_stk, b / a);
			break;

		default:
			real_t_stack_push (proc->real_stk, b);
			real_t_stack_push (proc->real_stk, a);

			proc->status = PROC_ERROR;
		
			return __LINE__;
	}

	if (!(cmd & MASK_JMP) )
		proc->ip++;

	return 0;
}

int int_alu (proc_t *proc, cmd_t cmd, char *dojump)
{
	if (!proc || proc->status != PROC_RUNNING)
		return __LINE__;

	int_t b    = 0;
	int_t a    = 0;
	*dojump    = 0;

	int_t_stack_pop  (proc->int_stk, &a);
	int_t_stack_pop  (proc->int_stk, &b);

	switch (cmd)
	{
		case CMD_JE:
			*dojump = (b == a) ? 1 : 0;
			break;

		case CMD_JNE:
			*dojump = (b != a) ? 1 : 0;
			break;

		case CMD_JB:
			*dojump = (b < a) ? 1 : 0;
			break;

		case CMD_JBE:
			*dojump = (b <= a) ? 1 : 0;
			break;

		case CMD_JA:
			*dojump = (b > a) ? 1 : 0;
			break;

		case CMD_JAE:
			*dojump = (b >= a) ? 1 : 0;
			break;

		case CMD_ADD:
			int_t_stack_push (proc->int_stk, a + b);
			break;

		case CMD_SUB: 
			int_t_stack_push (proc->int_stk, b - a);
			break;

		case CMD_MUL:
			int_t_stack_push (proc->int_stk, b * a);
			break;

		case CMD_DIV:
			int_t_stack_push (proc->int_stk, b / a);
			break;

		default:
			int_t_stack_push (proc->int_stk, b);
			int_t_stack_push (proc->int_stk, a);

			proc->status = PROC_ERROR;
		
			return __LINE__;
	}
	
	if (!(cmd & MASK_JMP) )
		proc->ip++;

	return 0;;
}

int jmp (proc_t *proc, cmd_t cmd, char dojump)
{
	if (!proc || !(proc->ram) || proc->status != PROC_RUNNING)
		return __LINE__;

	if (cmd & MASK_REAL)
		cmd ^= MASK_REAL;

	switch (cmd)
	{
		case CMD_JMP:
		case CMD_JE:
		case CMD_JNE:
		case CMD_JB:
		case CMD_JA:
		case CMD_JAE:
		case CMD_CALL:
			break;

		default:
			proc->status = PROC_ERROR;
			
			return __LINE__;
	}

	if (!dojump)
	{
		proc->ip += 2;

		return 0;
	}

	if (cmd == CMD_CALL)
		cmd_t_stack_push (proc->ret_stk, proc->ip + 2);

	ram_read (proc->ram, proc->ip + 1, &proc->ip);

	return 0;
}

int push (proc_t *proc, cmd_t cmd)
{
	if (!proc || proc->status != PROC_RUNNING)
		return __LINE__;

	cmd_t arg   = 0;
	cmd_t value = 0;

	ram_read (proc->ram, proc->ip + 1, &arg);

	if (cmd & MASK_REAL)
		switch (cmd) 
		{
			case CMD_PUSHR_C:
				real_t_stack_push (proc->real_stk, *( (real_t *) &arg) );
				break;

			case CMD_PUSHR_R:
				if (arg < REGISTERS)
					real_t_stack_push (proc->real_stk, proc->real_reg[arg]);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}
				break;

			case CMD_PUSHR_RAM_C:
				ram_read (proc->ram, arg, &value);
				real_t_stack_push (proc->real_stk, *( (real_t *) &value) );
				break;
				
			case CMD_PUSHR_RAM_R:
				if (arg < REGISTERS)
					ram_read (proc->ram, proc->int_reg[arg], &value);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}

				real_t_stack_push (proc->real_stk, *( (real_t *) &value) );
				break;

			default:
				proc->status = PROC_ERROR;

				return __LINE__;
		}
	else
		switch (cmd)	
		{
			case CMD_PUSH_C:
				int_t_stack_push (proc->int_stk, *( (int_t *) &arg) );
				break;

			case CMD_PUSH_R:
				if (arg < REGISTERS)
					int_t_stack_push (proc->int_stk, proc->int_reg[arg]);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}
				break;

			case CMD_PUSH_RAM_C:
				ram_read (proc->ram, arg, &value);
				int_t_stack_push (proc->int_stk, *( (int_t *) &value) );
				break;
				
			case CMD_PUSH_RAM_R:
				if (arg < REGISTERS)
					ram_read (proc->ram, proc->int_reg[arg], &value);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}

				int_t_stack_push (proc->int_stk, *( (int_t *) &value) );
				break;

			default:
				proc->status = PROC_ERROR;

				return __LINE__;
		}

	proc->ip += 2;

	return cmd;
}

int pop (proc_t *proc, cmd_t cmd)
{
	if (!proc || !(proc->ram) || proc->status != PROC_RUNNING)
		return __LINE__;

	cmd_t arg   = 0;
	cmd_t value = 0;

	ram_read (proc->ram, proc->ip + 1, &arg);

	if (cmd & MASK_REAL)
		switch (cmd)
		{
			case CMD_POPR:
				if (arg < REGISTERS)	
					real_t_stack_pop (proc->real_stk, proc->real_reg + arg);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}	
				break;

			case CMD_POPR_RAM_C:
				real_t_stack_pop (proc->real_stk, (real_t *) &value);
				ram_write (proc->ram, arg, value);
				break;

			case CMD_POPR_RAM_R:
				if (arg >= REGISTERS)
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}

				real_t_stack_pop (proc->real_stk, (real_t *) &value);
				ram_write (proc->ram, proc->int_reg[arg], value);
				break;

			default:
				proc->status = PROC_ERROR;

				return __LINE__;
		}
	else
		switch (cmd)
		{
			case CMD_POP:
				if (arg <= REGISTERS)	
					int_t_stack_pop (proc->int_stk, proc->int_reg + arg);
				else
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}	
				break;

			case CMD_POP_RAM_C:
				int_t_stack_pop (proc->int_stk, (int_t *) &value);
				ram_write (proc->ram, arg, value);
				break;

			case CMD_POP_RAM_R:
				if (arg >= REGISTERS)
				{
					proc->status = PROC_ERROR;

					return __LINE__;
				}

				int_t_stack_pop (proc->int_stk, (int_t *) &value);
				ram_write (proc->ram, proc->int_reg[arg], value);
				break;

			default:
				proc->status = PROC_ERROR;

				return __LINE__;
		}

	proc->ip += 2;

	return 0;
}
