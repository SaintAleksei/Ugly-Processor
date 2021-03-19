#ifndef BINDUMP_H_INCLUDED
#define BINDUMP_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../commands.h"
#include "../tools/tools.h"

#define MAX_NAME_SIZE 64

char *decode_cmd (char *dest, cmd_t cmd);

#endif
