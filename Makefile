asm:
	gcc assembler/main.c assembler/assembler.c dynarr/dynarr.c tools/tools.c -Wall -Wextra -o asm

proc:
	gcc processor/main.c processor/processor.c stack/stack.c tools/tools.c ram/ram.c -Wall -Wextra -lm -o proc

bdump:
	gcc bindump/main.c bindump/bindump.c tools/tools.c -Wall -Wextra -o bdump

clean:
	rm *.bin *.dump *.log *.o
