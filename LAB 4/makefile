all: Task1 Task4


Task1: Task1.o
	gcc -g -m32 -Wall -o Task1 Task1.o
Task1.o: Task1.c
	gcc -m32 -g -Wall -c -o Task1.o Task1.c
Task4:
	gcc -g -m32 -Wall -fno-pie -fno-stack-protector -o Task4 Task4.c

.PHONY: clean
clean:
	rm -f *.o Task1 Task4