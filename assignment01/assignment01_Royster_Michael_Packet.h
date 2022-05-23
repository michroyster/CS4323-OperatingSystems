#ifndef PACKET_H
#define PACKET_H

#include "assignment01_Royster_Michael_Gift.h"

typedef struct Packet
{
    int size;
    int processid;
    float totalCost;
    char order;
    Gift gifts[20];
} Packet;

#endif