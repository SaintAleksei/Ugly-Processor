#include "tools/tools.h"

int main ()
{
	for (;;)
	{
		char buffer[64] = "";

		scanf ("%s", buffer);

		hash_t hash = get_hash (buffer, 64);

		printf ("Hash is 0x%08llx\n", hash);
	}

	return 0;
}
