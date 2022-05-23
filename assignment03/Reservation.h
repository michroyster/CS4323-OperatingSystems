#ifndef RESERVATION_H
#define RESERVATION_H

// make a struct for Reservation

typedef struct Reservation{
    char customerName[64];
    char dob[32];
    char gender[12];
    int govID;
    char travelDate[32];
    int numberTravelers;
    char seat[4];
    char ticket_number[24];
} Reservation;

#endif