// Author: Michael Royster
#ifndef BACKEND_H
#define BACKEND_H

#include "Reservation.h"

void    init_sync(sem_t *file_write, sem_t *file_read, int shm_fd, int *ptrReaders);
void    desync(sem_t *file_write, sem_t *file_read, int shm_fd, int *ptrReaders);
void    ServerX(char name);
void    get_date(char* date, char* tomorrow);
int     file_exists(char *filename);
void    make_reservation(char server, Reservation* reservation, int numberTravelers);
void    add_travelers(char server, Reservation* reservation, int numberNewTravelers, char* ticket);
void    remove_traveler(char* ticket, char* name);
void    inquiry(char* ticket, Reservation* info, int* numTravelers);
void    update_train_seats(char* ticket,char* name, char* seat, char server); // modify
void    cancel_reservation(char* ticket);
//void    available_seats(int date, char* options, int* numberAvailable);
void 	available_seats(char *date, char* options, int* numberAvailable);
void    receipt(Reservation *reservations, int numberTravelers, char server, char* receipt);
//int     check_seat(int date, char *seat);
int 	check_seat(char *date, char *seat);
void    testX(char name);
void 	get_num_travelers(char* ticket, int* numTravelers);
void 	get_travel_date(char* ticket, char* travelDate);

#endif