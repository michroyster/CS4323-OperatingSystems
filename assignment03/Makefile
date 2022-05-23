main:
	gcc -c -o server.o Server.c -pthread -lrt
	gcc -c -o backend.o Backend.c -pthread -lrt
	gcc -c -o main.o Main.c
	gcc -o main main.o server.o backend.o -pthread -lrt

client:
	gcc -o client Client.c -lrt
	
clean:
	rm *.o

remove:
	rm /dev/shm/sem.file_write
	rm /dev/shm/sem.file_read

test:
	gcc -c -o backend.o Backend.c -pthread -lrt
	gcc -c -o test.o Test.c -pthread -lrt
	gcc -o test test.o backend.o -pthread -lrt