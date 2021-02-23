enum COMMANDS
{
	CMD_HLT,
	CMD_PUSH_CONST,
	CMD_PUSH_REG,
	CMD_POP,
	CMD_ADD,
	CMD_SUB,
	CMD_MUL,
	CMD_DIV,
	CMD_FSQRT,
	CMD_IN,
	CMD_OUT,
	CMD_NOP,
	CMD_JMP,
};

enum REGISTERS
{
	REG_R1,
	REG_R2,
	REG_R3,
	REG_R4,
};

typedef unsigned long long cmd_t;
