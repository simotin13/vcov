all: cpu.o
	g++ -g main.cpp cpu.o

cpu.o:
	nasm -f elf64 cpu.asm	

clean:
	rm -f *.o a.out

