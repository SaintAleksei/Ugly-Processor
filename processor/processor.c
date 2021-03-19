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

	proc->ip     = 0;
	proc->dojump = 0;
		
	for (size_t i = 0; i < REGISTERS; i++)
		proc->reg [i] = 0;

	proc->int_stk  = int_t_stack_ctor   (1);
	proc->real_stk = real_t_stack_ctor  (1);
	proc->ret_stk  = cmd_t_stack_ctor   (1);

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
		proc->dojump = 0;
		cmd_t cmd    = 0;
	
		ram_read (proc->ram, proc->ip, &cmd);

		if (cmd & MASK_ALU)
			run_alu (proc, cmd);
		else if (cmd & MASK_PUSH)
			do_push (proc, cmd);
		else if (cmd & MASK_POP)
			do_pop  (proc, cmd);
		else 
			do_cmd  (proc, cmd); 

		if (proc->dojump)
		{
			ram_read (proc->ram, proc->ip, &cmd);

			proc->ip = cmd;
		}

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

int run_alu (proc_t *proc, cmd_t cmd)
{
	if (!proc)
		return __LINE__;

	proc->dojump = 0;

	if (cmd == CMD_FSQRT)
	{
		if (stack_size (proc->real_stk) < 1)
		{
			fprintf (stderr, "PROCESSOR: RUN_ALU: [ERROR]: Can't pop value from real stack:\n"
							 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);
		
			return __LINE__;
		}
		else 
		{
			real_t a = 0;
			real_t_stack_pop  (proc->real_stk, &a);
			real_t_stack_push (proc->real_stk, a); 
		}

		return 0;
	}
	
	int iserror = 0;

	if (cmd & MASK_REAL)
	{
		real_t a = 0;
		real_t b = 0;

		if (stack_size (proc->real_stk) < 2)
		{
			fprintf (stderr, "PROCESSOR: RUN_ALU: [ERROR]: Can't pop value from real stack:\n"
							 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);

			return __LINE__;
		}
		else
		{
			real_t_stack_pop (proc->real_stk, &a);
			real_t_stack_pop (proc->real_stk, &b);
		}

		if (cmd & MASK_JMP)
			switch (cmd)
			{
				case CMD_JER:
					if (a == b)
						proc->dojump = 1;
					break;

				case CMD_JNER:
					if (a != b)
						proc->dojump = 1;
					break;

				case CMD_JLR:
					if (b < a)
						proc->dojump = 1;
					break;

				case CMD_JLER:
					if (b <= a)
						proc->dojump = 1;
					break;

				case CMD_JGR:
					if (b > a)
						proc->dojump = 1;
					break;

				case CMD_JGER:
					if (b >= a)
						proc->dojump = 1;
					break;

				default:
					iserror = __LINE__;
			}
		else
			switch (cmd)
			{
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
					iserror = __LINE__;
			}

		if (iserror)
		{
			real_t_stack_push (proc->real_stk, b);
			real_t_stack_push (proc->real_stk, a);
		
			proc->status = PROC_ERROR;

			return iserror;
		}
	}
	else 
	{
		int_t  a = 0;
		int_t  b = 0;

		if (stack_size (proc->int_stk) < 2)
		{
			fprintf (stderr, "PROCESSOR: RUN_ALU: [ERROR]: Can't pop value from int stack:\n"
							 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);

			return __LINE__;
		}
		else
		{
			int_t_stack_pop (proc->int_stk, &a);
			int_t_stack_pop (proc->int_stk, &b);
		}

		if (cmd & MASK_JMP)
			switch (cmd)
			{
				case CMD_JE:
					if (a == b)
						proc->dojump = 1;
					break;

				case CMD_JNE:
					if (a != b)
						proc->dojump = 1;
					break;

				case CMD_JL:
					if (b < a)
						proc->dojump = 1;
					break;

				case CMD_JLE:
					if (b <= a)
						proc->dojump = 1;
					break;

				case CMD_JG:
					if (b > a)
						proc->dojump = 1;
					break;

				case CMD_JGE:
					if (b >= a)
						proc->dojump = 1;
					break;

				default:
					iserror = __LINE__;
			}
		else
			switch (cmd)
			{
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
					iserror = __LINE__;
			}

		if (iserror)
		{
			int_t_stack_push (proc->int_stk, b);
			int_t_stack_push (proc->int_stk, a);
		
			proc->status = PROC_ERROR;

			return iserror;
		}
	}

	if (!iserror)
	{
		proc->ip++;

		if (!(proc->dojump) && cmd & MASK_JMP)
			proc->ip++;

		return 0;
	}
	else
		return __LINE__;
}

int do_push (proc_t *proc, cmd_t cmd)
{
	if (!proc)
		return __LINE__;

	int iserror = 0;
	
	cmd_t arg = 0;
	ram_read (proc->ram, proc->ip + 1, &arg);

	if (cmd & MASK_REAL)
		switch (cmd)
		{
			case CMD_PUSHR_C:
				real_t_stack_push (proc->real_stk, *( (real_t *) &arg ) );
				break;

			case CMD_PUSHR_R:
				if (arg < REGISTERS)
					real_t_stack_push (proc->real_stk, *( (real_t *) (proc->reg + arg) ) );
				else
					iserror = __LINE__;
				break;

			case CMD_PUSHR_RAM_C:
				ram_read (proc->ram, arg, &arg);
				real_t_stack_push (proc->real_stk, *( (real_t *) &arg) );
				break;

			case CMD_PUSHR_RAM_R:
				if (arg < REGISTERS)
				{
					ram_read (proc->ram, proc->reg[arg], &arg);
					real_t_stack_push (proc->real_stk, *( (real_t *) &arg) );
				}
				else
					iserror = __LINE__;
				break;

			default:
				iserror = __LINE__;
		}	
	else
	{
		switch (cmd)
		{
			case CMD_PUSH_C:
				int_t_stack_push (proc->int_stk, *( (int_t *) &arg ) );
				break;

			case CMD_PUSH_R:
				if (arg < REGISTERS)
					int_t_stack_push (proc->int_stk, *( (int_t *) (proc->reg + arg) ) );
				else
					iserror = __LINE__;
				break;

			case CMD_PUSH_RAM_C:
				ram_read (proc->ram, arg, &arg);
				int_t_stack_push (proc->int_stk, *( (int_t *) &arg) );
				break;

			case CMD_PUSH_RAM_R:
				if (arg < REGISTERS)
				{
					ram_read (proc->ram, proc->reg[arg], &arg);
					int_t_stack_push (proc->int_stk, *( (int_t *) &arg) );
				}
				else
					iserror = __LINE__;
				break;

			default:
				iserror = __LINE__;
		}	
	}

	if (iserror)
	{
		proc->status = PROC_ERROR;

		return iserror;
	}
	else
	{
		proc->ip += 2;

		return 0;
	}
}

int do_pop  (proc_t *proc, cmd_t cmd)
{
	if (!proc)
		return __LINE__;

	int iserror = 0;
	
	cmd_t arg    = 0;
	ram_read (proc->ram, proc->ip + 1, &arg);

	if (cmd & MASK_REAL)
	{
		real_t a = 0;

		switch (cmd)
		{
			case CMD_POPR:
				if (arg < REGISTERS)
					real_t_stack_pop (proc->real_stk, (real_t *) (proc->reg + arg) );
				else
					iserror = __LINE__;
				break;

			case CMD_POPR_RAM_C:
				real_t_stack_pop (proc->real_stk, &a);
				ram_write (proc->ram, arg, *( (cmd_t *) &a) );
				break;

			case CMD_POPR_RAM_R:
				if (arg < REGISTERS)
				{
					real_t_stack_pop (proc->real_stk, &a);
					ram_write (proc->ram, proc->reg[arg], *( (cmd_t *) &a) );
				}
				else
					iserror = __LINE__;
				break;

			default:
				iserror = __LINE__;
		}
	}
	else
	{
		int_t a = 0;

		switch (cmd)
		{
			case CMD_POP:
				if (arg < REGISTERS)
					int_t_stack_pop (proc->int_stk, (int_t *) (proc->reg + arg) );
				else
					iserror = __LINE__;
				break;

			case CMD_POP_RAM_C:
				int_t_stack_pop (proc->int_stk, &a);
				ram_write (proc->ram, arg, *( (cmd_t *) &a) );
				break;

			case CMD_POP_RAM_R:
				if (arg < REGISTERS)
				{
					int_t_stack_pop (proc->int_stk, &a);
					ram_write (proc->ram, proc->reg[arg], *( (cmd_t *) &a) );
				}
				else
					iserror = __LINE__;
				break;

			default:
				iserror = __LINE__;
		}
	}

	if (iserror)
	{
		proc->status = PROC_ERROR;

		return iserror;
	}
	else
	{
		proc->ip += 2;

		return 0;
	}
}

int do_cmd  (proc_t *proc, cmd_t cmd)
{
	if (!proc)
		return __LINE__;

	int iserror  = 0;
	cmd_t buffer = 0;

	switch (cmd)
	{
		case CMD_JMP:
			proc->dojump = 1;

			proc->ip++;
			break;

		case CMD_CALL:
			proc->dojump = 1;
			cmd_t_stack_push (proc->ret_stk, proc->ip + 2);

			proc->ip++;
			break;

		case CMD_RET:
			if (stack_size (proc->ret_stk) == 0)
			{
				fprintf (stderr, "PROCESSOR: DO_CMD: [ERROR]: Can't pop value from ret stack:\n"
						    	 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);			
			
				iserror = __LINE__;
			}
			else
			{
				cmd_t_stack_pop (proc->ret_stk, &buffer);
				proc->ip = buffer;
			}
			break;
	
		case CMD_IN:
			printf ("#> ");
			scanf  ("%lld", (int_t *) &buffer);
			int_t_stack_push (proc->int_stk, *( (int_t *) &buffer) );
			
			proc->ip++;
			break;

		case CMD_INR:
			printf ("#> ");
			scanf  ("%lg", (real_t *) &buffer);
			real_t_stack_push (proc->real_stk, *( (real_t *) &buffer) );

			proc->ip++;
			break;

		case CMD_OUT:	
			if (stack_size (proc->int_stk) == 0)
			{
				fprintf (stderr, "PROCESSOR: DO_CMD: [ERROR]: Can't pop value from int stack:\n"
						    	 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);			
				iserror = __LINE__;
			}
			else
			{
				int_t_stack_pop (proc->int_stk, (int_t *) &buffer);
				printf ("@> %lld\n", *( (int_t *) &buffer) );
				int_t_stack_push (proc->int_stk, *( (int_t *) &buffer) );

				proc->ip++;
			}
			break;

		case CMD_OUTR:
			if (stack_size (proc->real_stk) == 0)
			{
				fprintf (stderr, "PROCESSOR: DO_CMD: [ERROR]: Can't pop value from real stack:\n"
						    	 "\tip = 0x%04llx; cmd = 0x%016llx;\n", proc->ip, cmd);			
				iserror = __LINE__;
			}
			else
			{
				real_t_stack_pop (proc->real_stk, (real_t *) &buffer);
				printf ("@> %lg\n", *( (real_t *) &buffer) );
				real_t_stack_push (proc->real_stk, *( (real_t *) &buffer) );

				proc->ip++;
			}
			break;

		case CMD_NOP:
			proc->ip++;
			break;

		case CMD_HLT:
			proc->status = PROC_HALTED;
			break;

		default:
			iserror = __LINE__; }
	
	if (iserror)
	{
		proc->status = PROC_ERROR;

		return iserror;
	}
	else
		return 0;
}
