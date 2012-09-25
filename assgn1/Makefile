warmup1: my402listappln.o my402list.o
	   gcc -o warmup1 -g my402listappln.o my402list.o

my402listappln.o: my402listappln.c my402list.h
	     gcc -g -c -Wall my402listappln.c

my402list.o: my402list.c my402list.h
	     gcc -g -c -Wall my402list.c

clean:
	rm -rf *.o warmup1
