TARGET=vcov
all: cpu.o
	g++ -g main.cpp cpu.o -o ${TARGET}

cpu.o:
	nasm -f elf64 cpu.asm	

clean:
	rm -f *.o ${TARGET}

