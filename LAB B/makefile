all: bubblesort AntiVirus


bubblesort: bubblesort.o 
	gcc -g -m32 -Wall -o bubblesort bubblesort.o 

bubblesort.o: bubblesort.c
	gcc -m32 -g -Wall -c -o bubblesort.o bubblesort.c

AntiVirus: AntiVirus.o 
	gcc -g -m32 -Wall -o AntiVirus AntiVirus.o 

AntiVirus.o: AntiVirus.c
	gcc -m32 -g -Wall -c -o AntiVirus.o AntiVirus.c

.PHONY:clean
clean:
	rm -f *.o bubblesort AntiVirus