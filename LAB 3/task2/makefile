all:task2
task2:start.o Util.o main.o
	ld -m elf_i386 start.o main.o Util.o -o task2
start.o:start.s
	nasm -f elf32 start.s -o start.o

Util.o:Util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector Util.c -o Util.o
main.o:main.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o
clean:
	rm -f *.o task2