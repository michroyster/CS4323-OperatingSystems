#include <stdio.h>
#include <string.h> 	
#include <unistd.h> 
#include <sys/types.h> 	
#include <sys/stat.h> 	
#include <fcntl.h> 		
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <arpa/inet.h> 	// client
#include <sys/socket.h>
#include <netinet/in.h> // server
#include <pthread.h>
#include <semaphore.h>

#define IP "127.0.0.1"
#define PORT 8019
#define BUFFER_SIZE 2048
#define SERVER_COUNT 5

int connect_to_server()
{
	int sock, i;
	struct sockaddr_in address;
	socklen_t addrlen;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		puts("Failed to create socket");
		return -1;
	}
	
	
	while (1)
	{
		for (i = 0; i < SERVER_COUNT; i++)
		{
			address.sin_family = AF_INET;
			address.sin_port = htons(PORT + i);
			char temp[20];
			sprintf(temp, "port %d", PORT+i);
			puts(temp);
			if (inet_pton(AF_INET, IP, &address.sin_addr) <= 0)
			{
				puts("Invalid address");
				return -1;
			}
			
			// TRY to connect to server i
			addrlen = sizeof(address);
			if (connect(sock, (struct sockaddr *)&address, addrlen) < 0) // failed to connect to server i
			{
				char temp[120];
				sprintf(temp, "Failed to connect to server %d", i + 1);
				puts(temp);
			}
			else // connected to server i
			{
				//puts("Connection successful");
				return sock;
			}
		}
		puts("All servers full, waiting 5 seconds and retrying");
		sleep(5);
	}
}


int main()
{
	char message[BUFFER_SIZE-24];
	char buffer[BUFFER_SIZE] = {0};
	char *exit = "Exiting";
	int sock = connect_to_server();
	if (sock < 0) puts("There was an error creating the client socket");
	
	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		recv(sock, buffer, BUFFER_SIZE, 0);
		puts(buffer);
		if (!strcmp(buffer, exit)) break;
		
		memset(message, 0, sizeof(message));
		fgets(message, BUFFER_SIZE-24, stdin);
		message[strlen(message)-1] = '\0';
		send(sock, message, BUFFER_SIZE-24, 0);
	}
	
	close(sock);
	puts("client exited");
	return 0;
}
	