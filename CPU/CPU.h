#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../commands.h"

typedef struct CPU
{
	cmd_t *inst;
	cmd_t pc;
	double reg[4];
	struct Stack * stk;
} cpu_t;

int CPURun (cpu_t *cpu); /* CPU running function */

int CPULoad (FILE *stream, cpu_t *cpu); /* CPU load function */
