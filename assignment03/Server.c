// Group: E
// Description: This program creates multiple threads for each server and handles client connections to them. It also handles communicating with the client and 
//	calling the Backend functions to write to the summary file and make receipts.
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
#include <time.h>
#include "Backend.h"
#include "Reservation.h"
#include "Server.h"

#define IP "127.0.0.1"
#define PORT 8019
#define BUFFER_SIZE 2048
#define THREAD_NUMBER 3
#define SEAT_CAPACITY 27

Client priority_queue[THREAD_NUMBER];

pthread_t thread_pool[THREAD_NUMBER];
pthread_t queue_t;

sem_t semaphores[THREAD_NUMBER];
sem_t queue, enforce_thread_limit;

int thread_in_use[THREAD_NUMBER] = {0};
int num_seats = 27, pq_size = 0, run = 1;

void example(int purchased_seats)
{
	char out[120];
	if (purchased_seats > num_seats)
	{
		// do something if there are not enough seats left
		puts("not enough seats left");
	}
	else 
	{
		// let the client access the seat list for selection/deselection
		num_seats -= purchased_seats;
		sprintf(out, "Number of seats remaining: %d", num_seats);
		puts(out);
	}
	sleep(7); 			// simulates client choosing seats
	sem_post(&queue); 	// signal to the queue that it can allow a new client to access the seat list
}

// insert a new member into the priority queue; at the correct spot relative to its priority
int enqueue(int index_n, int priority_n)
{
	for (int i = 0; i < THREAD_NUMBER; i++)
	{
		// if the current index in the priority queue is empty; just insert
		// means that it is the lowest priority or the first
		// probably not needed
		if (priority_queue[i].index == -1)
		{
			priority_queue[i].index = index_n;
			priority_queue[i].priority = priority_n;
			pq_size++;
			break;
		}
		if (priority_queue[i].priority < priority_n)
		{
			int temp = pq_size;
			while (temp > i)
			{
				// start at the cell after the last populated cell;
				// shift each member to the right by 1 UNTIL
				// you reach the insertion point of the newly enqueued member
				priority_queue[temp].index = priority_queue[temp - 1].index;
				priority_queue[temp].priority = priority_queue[temp - 1].priority;
				temp--;
			}
			// insert the new member
			priority_queue[i].index = index_n;
			priority_queue[i].priority = priority_n;
			pq_size++;
			break;
		}
	}
}

// increase the priority of all members of the queue by 1
void increase_priority()
{
	for (int i = 0; i < pq_size; i++)
	{
		if (priority_queue[i].index != -1) priority_queue[i].priority++;
	}
}

// remove a member from the queue at the specified index (will always be 0 in our use case though)
void dequeue(int i)
{
	if (i < THREAD_NUMBER)
	{
		while (i < pq_size)
		{
			// shift all members > i to the left
			priority_queue[i].index = priority_queue[i + 1].index;
			priority_queue[i].priority = priority_queue[i + 1].priority;
			i++;
		}
		pq_size--; // decrease size of queue
		priority_queue[pq_size].index = -1; // set last elements index to -1 (checked elsewhere)
		increase_priority(); // increase_priority of all current members
	}
}

// returns the member of the queue with the highest priority (the one at index 0)
int get_highest_priority()
{
	int index_n = priority_queue[0].index;
	int priority_n = priority_queue[0].priority;
	char temp[40];
	sprintf(temp, "index %d : priority %d", index_n, priority_n);
	puts(temp);
	dequeue(0);
	return index_n;
}

// thread for handling the priority queue
void *queue_thread(void *arg)
{
	// calls sem_wait on each thread's semaphore so that the thread will actually wait 
	for (int i = 0; i < THREAD_NUMBER; i++) sem_wait(&semaphores[i]);
	while(1)//while(run)
	{
		if (pq_size > 0) // busy wait; not ideal but need some condition here
		{
			// this could potentially cause an issue if the client disconnects early; not sure if we are meant to handle that or not
			sem_wait(&queue); // wait for the queue to be ready to use 
			int index = get_highest_priority(); // get the index of the thread with the highest priority
			sem_post(&semaphores[index]); // allow that thread to access the seat list
		}
	}
	pthread_exit(NULL);
}

// server thread; there will be  THREAD_NUMBER of these possible per server; handles communicating with client and calling the relevant Backend functions
void *server_thread(void *arg)
{
	int purchased_seats, sock, client_choice, id_index, priority;
	char buffer[BUFFER_SIZE] = {0};
	char message[BUFFER_SIZE - 24];
	
	char *temp = (char *)arg;
	char *rest = temp;
	char *temp1 = strtok_r(temp, ",", &rest);
	char *temp2 = strtok_r(NULL, ",", &rest);
	char *temp3 = strtok_r(NULL, ",", &rest);
	char name = *temp3;
	
	sock = atoi(temp1);
	id_index = atoi(temp2);
	
	// Interact with client here
	while (1)
	{
		// send menu to client
		char* menu = "1. Make a reservation\n2. Inquiry about the ticket\n3. Modify the reservation\n4. Cancel the reservation\n5. Exit the program";
		char* makeRes = "How many travelers: ";
		char* getName = "Enter passenger name: ";
		char* getGender = "Enter passenger gender: ";
		char* getDOB = "Enter passenger DOB: ";
		char* getGovID = "Enter passenger gov ID: ";
		//char* getTravelDate = "Enter travel date (ex: MMDDYYYY): ";
		char* getTravelDate = "Enter travel date (today or tomorrow): ";
		char today[32];
		char tomorrow[32];
		
		memset(buffer, 0, sizeof(buffer));
		send(sock, menu, BUFFER_SIZE, 0);

		// receive client choice 
		recv(sock, buffer, BUFFER_SIZE, 0);
		client_choice = atoi(buffer);
		// these functions will need to communicate with the user according to their specifications; and parse their input into variables
		if (client_choice == 1) { // Make reservation
			// Get # travelers
			memset(buffer, 0, sizeof(buffer));
			send(sock, makeRes, BUFFER_SIZE-24, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
			int numTravelers = atoi(buffer);
			Reservation* reservations = (Reservation*)malloc(sizeof(Reservation)*numTravelers);
			
			get_date(today, tomorrow);
			
			// get Travel date
			memset(buffer, 0, sizeof(buffer));
			send(sock, getTravelDate, BUFFER_SIZE, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
			char travelDate[32];
			while (1)
			{
				if (!strcasecmp(buffer, "today")) 
				{
					strcpy(travelDate, today);
					break;
				}
				else if (!strcasecmp(buffer, "tomorrow")) 
				{
					strcpy(travelDate, tomorrow);
					break;
				}
				else 
				{
					memset(buffer, 0, sizeof(buffer));
					send(sock, getTravelDate, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
				}
			}
			//int travelDate = atoi(buffer);
			
			// Enough seats?
			int seatCount;
			char arr[112];
			available_seats(travelDate, arr, &seatCount);
			
			char tete[120];
			sprintf(tete, "==%d==", seatCount);
			puts(tete);
			
			if (seatCount >= numTravelers){
				enqueue(id_index, numTravelers);
				sem_wait(&semaphores[id_index]);
				
				memset(arr, 0, sizeof(arr));
				available_seats(travelDate, arr, &seatCount);
				memset(tete, 0, sizeof(tete));
				sprintf(tete, "==%d==", seatCount);
				puts(tete);
				if (seatCount < numTravelers)
				{
					char* notEnough = "There are no longer enough seats! Disconnecting\nPress Enter to exit.";
					send(sock, notEnough, BUFFER_SIZE-24, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					break;
				}
				
				// Get info
				for (int i = 0; i < numTravelers; i++){
					//char tmp[120];
					//sprintf(tmp, "%d", travelDate);
					//strcpy((reservations+i)->travelDate, tmp);
					strcpy((reservations+i)->travelDate, travelDate);
					//*(reservations+i)->travelDate = travelDate;
					// get name
					memset(buffer, 0, sizeof(buffer));
					send(sock, getName, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					strcpy((reservations+i)->customerName, buffer);
					// get dob
					memset(buffer, 0, sizeof(buffer));
					send(sock, getDOB, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					strcpy((reservations+i)->dob, buffer);
					// get gender
					memset(buffer, 0, sizeof(buffer));
					send(sock, getGender, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					strcpy((reservations+i)->gender, buffer);
					// get govID
					memset(buffer, 0, sizeof(buffer));
					send(sock, getGovID, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					(reservations+i)->govID = atoi(buffer);
				}
				char* confirmMessage = "Do you want to make reservation (yes/no): ";
				memset(buffer, 0, sizeof(buffer));
				send(sock, confirmMessage, BUFFER_SIZE, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				puts(buffer);
				char chosen_seats[55]; // max number possible *2 +1
				memset(chosen_seats, 0, sizeof(chosen_seats));
				if (strcmp(buffer, "yes") == 0){
					for(int j = 0; j < numTravelers; j++){
						char seats[112];
						int trav_temp;
						available_seats((reservations+j)->travelDate, seats, &trav_temp);
						//available_seats(atoi((reservations+j)->travelDate), seats, &trav_temp);
						char *seatMessage = "Pick your seat: ";
						memset(message, 0, sizeof(message));
						sprintf(message, "%s\n%s", seats, seatMessage);
						send(sock, message, BUFFER_SIZE-24, 0);
						
						memset(buffer, 0, sizeof(buffer));
						recv(sock, buffer, BUFFER_SIZE, 0);
						strcpy((reservations+j)->seat, buffer);
						puts(buffer);
						
						if (!check_seat(travelDate, buffer) || strstr(chosen_seats, buffer) != NULL || !strcmp(chosen_seats, buffer))
						{
							memset(message, 0, sizeof(message));
							sprintf(message, "Seat %s is not available, please pick another seat.\nPress Enter to try again.", buffer);
							send(sock, message, BUFFER_SIZE-24, 0);
							recv(sock, buffer, BUFFER_SIZE, 0);
							j--;
						}
						else strcat(chosen_seats, buffer);
						//puts(chosen_seats);
						//else add_travelers(name, (reservations+j), 1, numTravelers);
					}
					make_reservation(name, reservations, numTravelers);
					memset(message, 0, sizeof(message));
					receipt(reservations, numTravelers, name, message);
					strcat(message, "\nPlease press Enter to return to main menu");
					send(sock, message, BUFFER_SIZE, 0);
					
					memset(buffer, 0, sizeof(buffer));
					recv(sock, buffer, BUFFER_SIZE-24, 0);
					
					free(reservations);
				}
				//available_seats(travelDate, arr, &seatCount);
			
				//tete[120];
				///sprintf(tete, "==Seatcount:%d==", seatCount);
				//puts(tete);
				sem_post(&queue);
			}else{
				char* notEnough = "There are not enough seats!\nPress Enter to return to menu.";
				send(sock, notEnough, BUFFER_SIZE-24, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				//break;
			}
		}
		else if (client_choice == 2){ // inquiry
			char* inq = "Enter your ticket number: ";
			Reservation* info = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
			send(sock, inq, BUFFER_SIZE, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
			int trav;
			inquiry(buffer, info, &trav);
			char info_message[BUFFER_SIZE];
			memset(info_message, 0, sizeof(info_message));
			char temp[BUFFER_SIZE] = "";
			memset(temp, 0, sizeof(temp));
			char as[120];
			memset(as, 0, sizeof(as));
			sprintf(as, "Number of travellers: %d\n", trav);
			strcat(info_message, as);
			for (int i = 0; i < trav; i++){
				memset(temp, 0, sizeof(temp));
				sprintf(temp, "%s\t%s\t%s\t%d\t%s\t%s\t%s\n", (info+i)->customerName, (info+i)->dob, (info+i)->gender, (info+i)->govID, (info+i)->travelDate, (info+i)->seat, (info+i)->ticket_number);
				strcat(info_message, temp);
				puts(temp);
			}
			strcat(info_message, "Press Enter to return to main menu.");
			send(sock, info_message, BUFFER_SIZE-24, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
		} 
		else if (client_choice == 3){
			char* modify_options = "Select which to modify\nA. Change Seat\nB. Change Travel Date\nC. Remove Travelers\nD. Add Travelers\n";
			send(sock, modify_options, BUFFER_SIZE, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
			if (buffer[0] == 'A'){ // Change seat
				enqueue(id_index, 1);
				sem_wait(&semaphores[id_index]);
				
				// get ticket
				char* inq = "Enter your ticket number: ";
				send(sock, inq, BUFFER_SIZE, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				char ticket[20] = "";
				strcpy(ticket, buffer);
				// get number of travelers on ticket
				Reservation* temp = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
				int trav;
				inquiry(ticket, temp, &trav);
				// get travelDate
				char travDate[32] = "";
				strcpy(travDate, temp->travelDate);
				// Enough seats?
				int seatCount;
				char arr[112];
				available_seats(travDate, arr, &seatCount);
				//available_seats(atoi(travDate), arr, &seatCount);

				char modifyName[BUFFER_SIZE-24] = "Enter name to be modified: \n";
				for (int i = 0; i < trav; i++){
					strcat(modifyName, (temp+i)->customerName);
					strcat(modifyName, "\n");
				}
				send(sock, modifyName, BUFFER_SIZE-24, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				char r_name[64];
				strcpy(r_name, buffer);
				// Reservation* info = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
				char seats[112];
				int temporary = 0;
				//available_seats(atoi(temp->travelDate), seats, &temporary);
				available_seats(temp->travelDate, seats, &temporary);
				if (seatCount <= 0){
					char* full = "Cannot change seats the train is full.\n";
					send(sock, full, BUFFER_SIZE, 0);
					sem_post(&queue);
					break;
				} else{
					char seat_msg[BUFFER_SIZE];
					sprintf(seat_msg, "%s\n%s: ", "Select an available seat", seats);
					send(sock, seat_msg, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					char newSeat[BUFFER_SIZE];
					strcpy(newSeat, buffer);
					update_train_seats(ticket, r_name, newSeat, name);
					
					char* complete_msg = "Modification complete.\nPress Enter to return to the menu.";
					send(sock, complete_msg, BUFFER_SIZE-24, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					free(temp);
					sem_post(&queue);
				}

			}
			else if (buffer[0] == 'B'){ // change travel date
				enqueue(id_index, 1);
				sem_wait(&semaphores[id_index]);

				// get ticket
				char* modTicket = "Enter ticket to be modified: ";
				send(sock, modTicket, BUFFER_SIZE, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				char ticket[BUFFER_SIZE-24];
				strcpy(ticket, buffer);
				
				get_date(today, tomorrow);
				
				char currDate[32];
				char newTravelDate[32];
				get_travel_date(ticket, currDate);
				
				if (!strcmp(currDate, today)) strcpy(newTravelDate, tomorrow);
				else if (!strcmp(currDate, tomorrow)) strcpy(newTravelDate, today);
				
				char msgNewDate[BUFFER_SIZE-24];
				sprintf(msgNewDate, "Your current travel date is %s, do you want to change it to %s? (yes/no)", currDate, newTravelDate);
				
				while (1)
				{
					memset(buffer, 0, sizeof(buffer));
					send(sock, msgNewDate, BUFFER_SIZE-24, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);

					if (!strcasecmp(buffer, "yes"))
					{
						Reservation* info = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
						int trav;
						inquiry(ticket, info, &trav);
						for (int i = 0; i < trav; i++){
							strcpy((info+i)->travelDate, newTravelDate);
						}
						char seats[112];
						int seatCount;
						available_seats(newTravelDate, seats, &seatCount);
						if (seatCount <= trav){
							char* full = "Cannot change seats the train is full.\n";
							send(sock, full, BUFFER_SIZE, 0);
							break;
						}else{
							cancel_reservation(ticket);
							for (int i = 0; i < trav; i++){
								int temporary1 = 0;
								available_seats((info+i)->travelDate, seats, &temporary1);
								char ttmp[BUFFER_SIZE-24];
								sprintf(ttmp, "Please choose your seats for %s:\n%s", newTravelDate, seats);
								send(sock, ttmp, BUFFER_SIZE-24, 0);
								
								recv(sock, buffer, BUFFER_SIZE, 0);
								strcpy((info+i)->seat, buffer);
								add_travelers(name, (info+i), 1, ticket);
							}
						}
						char* msgDatChanged = "Date has been changed.\nPress Enter to return to menu.";
						send(sock, msgDatChanged, BUFFER_SIZE-24, 0);
						recv(sock, buffer, BUFFER_SIZE, 0);
						free(info);
						break;
					}
					else if (!strcasecmp(buffer, "no")) break;
				}
				sem_post(&queue);
			}
			else if (buffer[0] == 'C'){ // remove travelers
				enqueue(id_index, 1);
				sem_wait(&semaphores[id_index]);

				// get ticket
				char* modTicket = "Enter ticket to be modified: ";
				send(sock, modTicket, BUFFER_SIZE, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				char ticket[BUFFER_SIZE-24];
				strcpy(ticket, buffer);
			
				// get number of travelers
				Reservation* info = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
				int trav;
				inquiry(ticket, info, &trav);
				//char* more = "yes";
				while(1)
				{
					char msg[BUFFER_SIZE-24] = "Which traveler would you like to remove?\n";
					for (int i = 0; i < trav; i++){
						strcat(msg, (info+i)->customerName);
						strcat(msg, "\n");
					}
					send(sock, msg, BUFFER_SIZE, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					char rmvName[BUFFER_SIZE-24];
					strcpy(rmvName, buffer);

					remove_traveler(ticket, rmvName);
					inquiry(ticket, info, &trav);

					char* again = "Remove another traveler? (yes/no) ";
					send(sock, again, BUFFER_SIZE-24, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
					if (strcasecmp(buffer, "yes")) break;
				}

				free(info);
				sem_post(&queue);
			}
			else if (buffer[0] == 'D'){ // add travelers
				enqueue(id_index, 1);
				sem_wait(&semaphores[id_index]);

				// get ticket
				char* modTicket = "Enter ticket to be modified: ";
				send(sock, modTicket, BUFFER_SIZE-24, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				char ticket[BUFFER_SIZE-24];
				strcpy(ticket, buffer);

				// get number of travelers on ticket
				Reservation* info = (Reservation*)malloc(sizeof(Reservation)*SEAT_CAPACITY);
				int trav;
				inquiry(ticket, info, &trav);
				// get travelDate
				char travDate[32];
				strcpy(travDate, info->travelDate);

				int seatsAvail;
				char seats[112];
				//available_seats(atoi(travDate), seats, &seatsAvail);
				available_seats(travDate, seats, &seatsAvail);
				
				char* msgHowMany = "How many travelers to add: ";
				send(sock, msgHowMany, BUFFER_SIZE-24, 0);
				recv(sock, buffer, BUFFER_SIZE, 0);
				int newTravelers = atoi(buffer);

				Reservation* newReservations = (Reservation*)malloc(sizeof(Reservation)*newTravelers);

				if (seatsAvail < newTravelers){
					char* msgStupid = "Not enough available seats\nPress Enter to return to menu.";
					send(sock, msgStupid, BUFFER_SIZE-24, 0);
					recv(sock, buffer, BUFFER_SIZE, 0);
				}else{
					// Get info
					for (int i = 0; i < newTravelers; i++){
						strcpy((newReservations+i)->travelDate, travDate);
						// get name
						send(sock, getName, BUFFER_SIZE, 0);
						recv(sock, buffer, BUFFER_SIZE, 0);
						strcpy((newReservations+i)->customerName, buffer);
						// get dob
						send(sock, getDOB, BUFFER_SIZE, 0);
						recv(sock, buffer, BUFFER_SIZE, 0);
						strcpy((newReservations+i)->dob, buffer);
						// get gender
						send(sock, getGender, BUFFER_SIZE, 0);
						recv(sock, buffer, BUFFER_SIZE, 0);
						strcpy((newReservations+i)->gender, buffer);
						// get govID
						send(sock, getGovID, BUFFER_SIZE, 0);
						recv(sock, buffer, BUFFER_SIZE, 0);
						(newReservations+i)->govID = atoi(buffer);
						
						int temporary1 = 0;
						available_seats((newReservations+i)->travelDate, seats, &temporary1);
						char ttmp[BUFFER_SIZE-24];
						sprintf(ttmp, "Please choose your seat %s:\n%s", (newReservations+i)->customerName, seats);
						send(sock, ttmp, BUFFER_SIZE-24, 0);	
						recv(sock, buffer, BUFFER_SIZE, 0);
						strcpy((newReservations+i)->seat, buffer);
						
						add_travelers(name, (newReservations+i), 1, ticket);
					}
				}
				free(info);
				free(newReservations);
				sem_post(&queue);
			}
			else{
				char* stupid = "Please enter a valid option!\n";
				send(sock, stupid, BUFFER_SIZE, 0);
			}
		} 
		else if (client_choice == 4) {
			char* cancelTicket = "Enter ticket number to cancel: ";
			send(sock, cancelTicket, BUFFER_SIZE, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
			cancel_reservation(buffer);
			char* cancelTicket2 = "Your ticket has been canceled.\nPress Enter to return to menu.";
			send(sock, cancelTicket2, BUFFER_SIZE-24, 0);
			recv(sock, buffer, BUFFER_SIZE, 0);
		}
		else if (client_choice == 5) break;
		// else send_error();
	}
	// send_exit_message();
	memset(message, 0, sizeof(message));
	send(sock, "Exiting", BUFFER_SIZE-24, 0);
	
	thread_in_use[id_index] = 0; 
	close(sock);
	sem_post(&enforce_thread_limit);
	puts("thread exited");
	pthread_exit(NULL);
}

// likely not worth the effort
/*
void *detect_quit(void *arg)
{
	char input[200];
	char out[210];
	//while (run)
	//{
		//scanf("%s", input);
		
		fgets(input, 200, stdin);
		sprintf(out, "++%s++", input);
		for (int i = 0; i < 200; i++)
		{
			if (input[i] == '\n') 
			{
				input[i] = '\0';
				break;
			}
		}
		if (!strncasecmp(input, "quit", 7))
		{
			run = 0;
			//pthread_cancel(queue_t);
			//for (int i = 0; i < THREAD_NUMBER; i++)
			//{
			//	pthread_cancel(thread_pool[i]);
			//}
		}
		else puts(out);
	//}
	strcpy(out, "");
	sprintf(out, "=%d=", run);
	puts(out);
	puts("quit exit");
	pthread_exit(NULL);
}
*/

// accepts a single client connection and assigns it a new socket; returns this to Server which will pass it off to a thread and then wait for either a 
// new client to connect; or for a thread to be open (depending on if all threads are full)
// forces the socket to recreate itself after admitting one client; this allows telling the client to try the next server when this one is full
// we cannot use the backlog of listen() for this purpose because it has a minimum of 16 (ie 16 clients could be waiting at once; ignoring the open servers)
int create_socket(int port)
{
	int server_fd, sock, opt = 1;
	struct sockaddr_in address;
	struct sockaddr_storage storage;
	socklen_t addrlen;
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		puts("Server socket creation failed");
		return -1;
	}
	//else puts("Server socket creation success");
	
	// this is necessary or the socket will fail to bind() on the second time through
	// from what I understand, by default it will wait a few minutes before fully closing the bound socket
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
		puts("setsockopt");
		return -1;
    }
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	
	if ((bind(server_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
	{
		puts("Server socket bind failed");
		return -1;
	}
	//else puts("Server socket bind success");
	
	if (listen(server_fd, 0) < 0) // might need to be 1 minimum; not sure (this is so we can have it try a different server 
	{
		puts("Server socket listen failed");
		return -1;
	}
	//else puts("Listening...");
	
	addrlen = sizeof(storage);
	if ((sock = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
	{
		puts("Server socket accept failed");
		return -1;
	}
	close(server_fd);
	return sock;
}

// Main function for servers; handles initializing the server-specific semaphores and threads; handles calling create_socket to accept new clients
int Server(char name, int port)
{
	srand(time(NULL));
	int thread_index = 0, i, sock;

	sem_init(&queue, 0, 1);
	sem_init(&enforce_thread_limit, 0, THREAD_NUMBER);
	for (i = 0; i < THREAD_NUMBER; i++) sem_init(&semaphores[i], 0, 1);
	
	pthread_create(&queue_t, NULL, queue_thread, NULL);
	
	while(1)//while (run)
	{
		sem_wait(&enforce_thread_limit); // wait for an open thread
		sock = create_socket(port); // create the socket for the new client
		if (sock < 0) return 1; // should probably break; instead
		
		// finds an open thread (we know one at least one is open because of the sem_wait() call above)
		while (thread_in_use[thread_index])// && run) // while the current thread is in use
		{
			thread_index++; // iterate through the array of threads
			if (thread_index >= THREAD_NUMBER) thread_index = 0; // if reach the end of the array; restart at the beginning
		}
		thread_in_use[thread_index] = 1; // declare that this thread is being used
		char temp[10];
		sprintf(temp, "%d,%d,%c", sock, thread_index, name);
		puts(temp);
		pthread_create(&thread_pool[thread_index], NULL, server_thread, (void *)temp); // create the server thread with the socket, thread index, and server name
	}
	sem_destroy(&queue);
	sem_destroy(&enforce_thread_limit);
	for (i = 0; i < THREAD_NUMBER; i++) sem_destroy(&semaphores[i]);
	puts("server exited");
	return 0;
}
