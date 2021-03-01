#include "CPU.h"

#define TYPE real_t
#include "../stack/stack.h"
#undef TYPE

#define TYPE int_t
#include "../stack/stack.h"
#undef TYPE

#define TYPE cmd_t
#include "../stack/stack.h"
#undef TYPE

int CPUDtor (cpu_t *cpu);

int CPULoad (FILE *stream, cpu_t *cpu)
{
	if (!stream || !cpu)
		return 1; /* Bad parameters */

	fseek (stream, 0, SEEK_END);
	size_t fsize = ftell (stream);
	fseek (stream, 0, SEEK_SET);

	fsize /= sizeof (cmd_t);

	cpu->inst = calloc (fsize, sizeof (cmd_t));

	if (fread (cpu->inst, sizeof (cmd_t), fsize, stream) != fsize)
	{
		free (cpu->inst);
		return 1; /* Trouble with fread */
	}

	cpu->pc = 0;

	for (size_t i = 0; i < INT_REGISTERS; i++)
		cpu->int_reg[i] = 0;	

	for (size_t i = 0; i < REAL_REGISTERS; i++)
		cpu->real_reg[i] = 0;

	cpu->int_stk   = int_t_stack_ctor (1);
	cpu->real_stk = real_t_stack_ctor (1);
	cpu->ret_stk   = cmd_t_stack_ctor (1);

	return 0; /* Success! */
}

int CPUDtor (cpu_t *cpu)
{
	if (!cpu)
		return 1; /* Bad parameters */

	int_t_stack_dtor   (&(cpu->int_stk) );
	real_t_stack_dtor (&(cpu->real_stk) );
	cmd_t_stack_dtor   (&(cpu->ret_stk) );
	free (cpu->inst); 
	return 0; /* Success */
}


int CPURun (cpu_t *cpu)
{
	if (!cpu || !cpu->inst || !cpu->int_stk || !cpu->real_stk || !cpu->ret_stk)
		return -1; /* Bad cpu */

	for (int stop = 0; !stop; cpu->pc++)
	{
		int_t a    = 0;
		int_t b    = 0;
		real_t fa = 0;
		real_t fb = 0;

		switch (cpu->inst[cpu->pc])
		{
			case CMD_PUSH_CONST:
				cpu->pc++;
				int_t_stack_push (cpu->int_stk, *( (int_t *) (cpu->inst + cpu->pc) ) );
				break;

			case CMD_PUSHR_CONST:
				cpu->pc++;
				real_t_stack_push (cpu->real_stk, *( (real_t *) (cpu->inst + cpu->pc) ) );
				break;

			case CMD_PUSH_REG:
				cpu->pc++;
				int_t_stack_push (cpu->int_stk, cpu->int_reg[cpu->inst[cpu->pc] ]);
				break;
		
			case CMD_PUSHR_REG:
				cpu->pc++;
				real_t_stack_push (cpu->real_stk, cpu->real_reg[cpu->inst[cpu->pc] ] );
				break;
				
			case CMD_POP:
				cpu->pc++;
				int_t_stack_pop (cpu->int_stk, cpu->int_reg + cpu->inst[cpu->pc]);
				break;

			case CMD_POPR:
				cpu->pc++;
				real_t_stack_pop (cpu->real_stk, cpu->real_reg + cpu->inst[cpu->pc] );
				break;

			case CMD_ADD:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				int_t_stack_push (cpu->int_stk, a + b);
				break;

			case CMD_ADDR:
				real_t_stack_pop (cpu->real_stk, &fa);
				real_t_stack_pop (cpu->real_stk, &fb);
				real_t_stack_push (cpu->real_stk, fa + fb);
				break;

			case CMD_SUB:
				int_t_stack_pop (cpu->int_stk, &a);	
				int_t_stack_pop (cpu->int_stk, &b);
				int_t_stack_push (cpu->int_stk, b - a);
				break;

			case CMD_SUBR:
				real_t_stack_pop (cpu->real_stk, &fa);
				real_t_stack_pop (cpu->real_stk, &fb);
				real_t_stack_push (cpu->real_stk, fb - fa);
				break;

			case CMD_MUL:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				int_t_stack_push (cpu->int_stk, b * a);
				break;

			case CMD_MULR:
				real_t_stack_pop (cpu->real_stk, &fa);
				real_t_stack_pop (cpu->real_stk, &fb);
				real_t_stack_push (cpu->real_stk, fb * fa);
				break;

			case CMD_DIV:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				int_t_stack_push (cpu->int_stk, b / a);
				break;

			case CMD_DIVR:
				real_t_stack_pop (cpu->real_stk, &fa);
				real_t_stack_pop (cpu->real_stk, &fb);
				real_t_stack_push (cpu->real_stk, fb / fa);
				break;

			case CMD_FSQRT:
				real_t_stack_pop (cpu->real_stk, &fa);
				real_t_stack_push (cpu->real_stk, sqrt (fa) );
				break;

			case CMD_RTI:
				real_t_stack_pop (cpu->real_stk, &fa);
				int_t_stack_push (cpu->int_stk, (int_t) fa);
				break;
		
			case CMD_ITR:
				int_t_stack_pop (cpu->int_stk, &a);
				real_t_stack_push (cpu->real_stk, (real_t) a);
				break;

			case CMD_IN:
				scanf ("%lld", &a);
				int_t_stack_push (cpu->int_stk, a);
				break;

			case CMD_INR:
				scanf ("%lg", &fa);
				real_t_stack_push (cpu->real_stk, fa);
				break;

			case CMD_GET:
				a = getc (stdin);
				int_t_stack_push (cpu->int_stk, a);
				break;

			case CMD_OUT:
				int_t_stack_pop (cpu->int_stk, &a);
				printf ("%lld\n", a);
				int_t_stack_push (cpu->int_stk, a);
				break;

			case CMD_OUTR:
				real_t_stack_pop (cpu->real_stk, &fa);
				printf ("%lg\n", fa);
				real_t_stack_push (cpu->real_stk, fa);
				break;
	
			case CMD_PUT:
				int_t_stack_pop (cpu->int_stk, &a);
				if (a >= -128 && a <= 127 && isgraph (a) )
					putc (a, stdout);
				break;

			case CMD_NOP:
				break;

			case CMD_CALL:
				cmd_t_stack_push (cpu->ret_stk, cpu->pc + 2);	
				cpu->pc = cpu->inst[cpu->pc + 1] - 1;
				break;

			case CMD_RET:
				cmd_t_stack_pop (cpu->ret_stk, (cmd_t *) &a);
				cpu->pc = *( (cmd_t *) &a) - 1;
				break;

			case CMD_JMP:
				cpu->pc = cpu->inst[cpu->pc + 1] - 1;
				break;

			case CMD_JE:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (a == b)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else
					cpu->pc++;
				break;

			case CMD_JNE:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (a != b)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else	
					cpu->pc++;
				break;

			case CMD_JB:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (b < a)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else	
					cpu->pc++;
				break;

			case CMD_JBE:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (b <= a)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else
					cpu->pc++;
				break;

			case CMD_JA:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (b > a)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else
					cpu->pc++;
				break;

			case CMD_JAE:
				int_t_stack_pop (cpu->int_stk, &a);
				int_t_stack_pop (cpu->int_stk, &b);
				if (b >= a)
					cpu->pc = cpu->inst[cpu->pc + 1] - 1;	
				else
					cpu->pc++;
				break;
	
			case CMD_HLT:
				stop++;
				break;

			default:
				return cpu->pc + 1; /* Unknown command */
		}
	}

	if (!CPUDtor (cpu) )
		return 0;
	else
		return -1;
}
