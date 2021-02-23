#include "CPU.h"

#define TYPE double
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

	for (size_t i = 0; i < 4; i++)
		cpu->reg[i] = 0;	

	cpu->stk = double_stack_ctor (1);

	return 0; /* Success! */
}

int CPUDtor (cpu_t *cpu)
{
	if (!cpu)
		return 1; /* Bad parameters */

	double_stack_dtor (&(cpu->stk) );
	free (cpu->inst); 
	return 0; /* Success */
}


int CPURun (cpu_t *cpu)
{
	if (!cpu || !cpu->inst || !cpu->stk)
		return -1; /* Bad cpu */

	for (int stop = 0, i = 0; !stop; cpu->pc++, i++)
	{
		double a = 0;
		double b = 0;

		switch (cpu->inst[cpu->pc])
		{
			case CMD_PUSH_CONST:
				cpu->pc++;
				double_stack_push (cpu->stk, *( (double *) (cpu->inst + cpu->pc) ) );
				break;

			case CMD_PUSH_REG:
				cpu->pc++;
				double_stack_push (cpu->stk, cpu->reg[cpu->inst[cpu->pc] ]);
				break;

			case CMD_POP:
				cpu->pc++;
				double_stack_pop (cpu->stk, cpu->reg + cpu->inst[cpu->pc]);
				break;

			case CMD_ADD:
				double_stack_pop (cpu->stk, &a);
				double_stack_pop (cpu->stk, &b);
				double_stack_push (cpu->stk, a + b);
				break;

			case CMD_SUB:
				double_stack_pop (cpu->stk, &a);	
				double_stack_pop (cpu->stk, &b);
				double_stack_push (cpu->stk, b - a);
				break;

			case CMD_MUL:
				double_stack_pop (cpu->stk, &a);
				double_stack_pop (cpu->stk, &b);
				double_stack_push (cpu->stk, b * a);
				break;

			case CMD_DIV:
				double_stack_pop (cpu->stk, &a);
				double_stack_pop (cpu->stk, &b);
				double_stack_push (cpu->stk, b / a);
				break;

			case CMD_FSQRT:
				double_stack_pop (cpu->stk, &a);
				double_stack_push (cpu->stk, (cmd_t) sqrt (a) );
				break;

			case CMD_IN:
				scanf ("%lg", &a);
				double_stack_push (cpu->stk, a);
				break;

			case CMD_OUT:
				double_stack_pop (cpu->stk, &a);
				printf ("%lg\n", a);
				double_stack_push (cpu->stk, a);
				break;

			case CMD_NOP:
				break;

			case CMD_JMP:
				cpu->pc = cpu->inst[cpu->pc + 1] - 1;
				break;
	
			case CMD_HLT:
				stop++;
				break;

			default:
				return cpu->pc + 1; /* Unknown command */
		}
	}

	return 0;
}
