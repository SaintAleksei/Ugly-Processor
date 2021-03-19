#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../commands.h"
#include "../tools/tools.h"

typedef struct processor
{
	cmd_t  ip;
	cmd_t  dp;
	cmd_t  reg[REGISTERS];
	unsigned char dojump;
	unsigned char status;
	struct RAM   *ram;
	struct Stack *int_stk;
	struct Stack *real_stk;
	struct Stack *ret_stk;
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

int run_alu  	(proc_t *proc, cmd_t cmd);
int do_jmp		(proc_t *proc, cmd_t cmd);
int do_push     (proc_t *proc, cmd_t cmd);
int do_pop      (proc_t *proc, cmd_t cmd);
int do_cmd      (proc_t *proc, cmd_t cmd);
