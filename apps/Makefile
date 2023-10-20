all:cpu.o
	gcc -g main.c cpu.o

cpu.o:
	nasm -f elf64 cpu.asm	

clean:
	rm -f *.o a.out

