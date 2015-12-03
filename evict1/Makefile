main: proj4_new.o
	gcc -o main proj4_new.o -pthread

proj4_new.o: proj4_new.c
	gcc proj4_new.c -lpthread -c

clean:
	rm *.o main