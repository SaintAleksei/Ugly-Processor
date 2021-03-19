#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../commands.h"
#include "../tools/tools.h"

typedef struct processor
{
	struct RAM *ram;
	cmd_t  ip;
	int_t  int_reg[REGISTERS];
	real_t real_reg[REGISTERS];
	struct Stack *int_stk;
	struct Stack *real_stk;
	struct Stack *ret_stk;
	unsigned char status;
} proc_t;

enum PROC_STATUSES
{
	PROC_DESTROYED,
	PROC_HALTED,
	PROC_RUNNING,
	PROC_ERROR,
};

int proc_load 	 (proc_t *proc, const char *fname);
int proc_run  	 (proc_t *proc);
int proc_destroy (proc_t *proc);
int proc_dump    (proc_t *proc);

int int_alu  	(proc_t *proc, cmd_t cmd, char *dojump);
int real_alu  	(proc_t *proc, cmd_t cmd, char *dojump);
int jmp			(proc_t *proc, cmd_t cmd, char dojump);
int push     	(proc_t *proc, cmd_t cmd);
int pop      	(proc_t *proc, cmd_t cmd);
