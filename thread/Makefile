main: proj4_thread1.o
	gcc -o main proj4_thread1.o -pthread

proj4_thread1.o: proj4_thread1.c
	gcc proj4_thread1.c -lpthread -c

clean:
	rm *.o main