main:
	gcc -o output Output.c -lrt
	gcc -o server Server.c -pthread
	gcc -c -o main.o Main.c -lrt
	gcc -c -o assistant.o Assistant.c -lrt
	gcc -c -o manager.o Manager.c -lrt
	gcc -c -o client.o Client.c -lrt
	gcc -c -o history.o History.c -lrt
	gcc -o main main.o assistant.o manager.o client.o history.o -lrt

clean:
	rm main
	rm output
	rm server
	rm ManagerPipe
	rm AssistantPipe
	rm *.o