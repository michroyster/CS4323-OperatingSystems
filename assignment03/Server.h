#ifndef SERVER_H
#define SERVER_H

typedef struct Client
{
	int index;
	int priority;
} Client;

int Server(char name, int port);
int enqueue(int idx, int prty);

#endif