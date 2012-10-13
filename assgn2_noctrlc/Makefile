warmup2: commandline.o warmup2.o my402list.o
	   gcc -o warmup2 -g commandline.o warmup2.o my402list.o -lpthread -lm

commandline.o: commandline.c packet.h
	     gcc -g -c -Wall commandline.c

warmup2.o: warmup2.c packet.h
	     gcc -g -c -Wall warmup2.c

my402list.o: my402list.c my402list.h
	     gcc -g -c -Wall my402list.c

clean:
	rm -rf *.o warmup2
