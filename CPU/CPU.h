#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "../commands.h"

typedef struct cpu
{
	cmd_t        *inst;
	cmd_t        pc;
	int_t        int_reg[INT_REGISTERS];
	real_t       real_reg[REAL_REGISTERS];
	struct Stack *int_stk;
	struct Stack *real_stk;
	struct Stack *ret_stk;
} cpu_t;

int CPURun (cpu_t *cpu); /* CPU running function */

int CPULoad (FILE *stream, cpu_t *cpu); /* CPU load function */
