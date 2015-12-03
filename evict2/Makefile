main: proj4_evict2.o
	gcc -o main proj4_evict2.o -pthread

proj4_new.o: proj4_evict2.c
	gcc proj4_evict2.c -lpthread -c

clean:
	rm *.o main