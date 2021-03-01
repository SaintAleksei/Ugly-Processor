project:
	gcc assembler/main.c -Wall -Wextra -o asm
	gcc CPU/main.c CPU/CPU.c stack/stack.c -Wall -Wextra -lm -o cpu

asm:
	gcc assembler/main.c -Wall -Wextra -o asm

cpu:
	gcc CPU/main.c CPU/CPU.c stack/stack.c -Wall -Wextra -lm -o cpu

clean:
	rm *.code
