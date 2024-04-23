# all:start
# start: start.o Util.o
# 	ld -m elf_i386 start.o Util.o -o start

# start.o: start.s
# 	nasm -f elf32 start.s -o start.o

# Util.o: Util.c
# 	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector Util.c -o Util.o

# clean:
# 	rm -f *.o start


all: start
start: Util.o
	nasm -f elf32 start.s -o start.o
	gcc -m32 Util.o start.o -o start

Util: Util.o
	nasm -g -f elf32 start.s -o start.o
	# gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task2.c -o task2.o
	# ld -g -m elf_i386 start.o task2.o util.o -o task2

Util.o:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector Util.c -o Util.o

.PHONY: clean
clean:
	rm -f *.o start 