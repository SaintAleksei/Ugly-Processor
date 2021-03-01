enum COMMANDS
{
	CMD_HLT,
	CMD_PUSH_CONST,
	CMD_PUSHR_CONST,
	CMD_PUSH_REG,
	CMD_PUSHR_REG,
	CMD_POP,
	CMD_POPR,
	CMD_ADD,
	CMD_ADDR,
	CMD_SUB,
	CMD_SUBR,
	CMD_MUL,
	CMD_MULR,
	CMD_DIV,
	CMD_DIVR,
	CMD_FSQRT,
	CMD_RTI,
	CMD_ITR,
	CMD_IN,
	CMD_INR,
	CMD_GET,
	CMD_OUT,
	CMD_OUTR,
	CMD_PUT,
	CMD_JMP,
	CMD_JE,
	CMD_JNE,
	CMD_JB,
	CMD_JBE,
	CMD_JA,
	CMD_JAE,
	CMD_CALL,
	CMD_RET,
	CMD_NOP,
};

typedef unsigned long long cmd_t;
typedef long long          int_t;
typedef double             real_t;

#ifndef REAL_REGISTERS
#define REAL_REGISTERS 4
#endif

#ifndef INT_REGISTERS
#define INT_REGISTERS 4
#endif
