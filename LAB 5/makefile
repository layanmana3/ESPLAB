all: loader


loader: loader.o start.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib/ld-linux.so.2

loader.o: task2.c
	gcc -m32 -Wall -c task2.c -o loader.o

start.o:start.s
	nasm -f elf32 start.s -o start.o

.PHONY: clean 

clean: 
	rm -f loader.o start.o loader
	


# task2:task2.c
# 	gcc -m32 -g -c task2.c -o task2.o
# 	ld -o task2 task2.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

# clean:
# 	rm -f task2 task2.o
# gcc -c loader.c -o loader.o
# ld -o loader loader.o startup.o start.o -lc -T linking_script -dynamic-linker /lib/ld-linux.so.2