all:cpu.o
	gcc test.c cpu.o

cpu.o:
	nasm -f elf64 cpu.asm	

clean:
	rm *.o a.out

