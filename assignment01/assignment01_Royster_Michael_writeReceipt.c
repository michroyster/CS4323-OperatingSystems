#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assignment01_Royster_Michael_Gift.h"
#include "assignment01_Royster_Michael_Packet.h"

// Takes in a packet and writes a receipt based on the information in the packet.

void writeReceipt(Packet packet){
    // Constructing filename
    char filename[20] = "receipt";
    int length = snprintf(NULL, 0, "%d", packet. processid);
    char *extension = malloc(length + 1);
    snprintf(extension, length + 1, "%d", packet.processid);
    strcat(filename, extension);
    strcat(filename, ".txt");

    // Opening file to write
    FILE *file = fopen(filename, "w");
    
    // Initializing buffer
    char buffer_out[512];

    // Writing Information to receipt
    sprintf(buffer_out, "%s%d : %c\n", "Process", packet.processid, packet.order);
    fwrite(buffer_out, sizeof(char), strlen(buffer_out), file);

    int i = 0;
    for (i = 0; i < packet.size; i++){
        sprintf(buffer_out, "%d %s %.2f %s\n", 
            packet.gifts[i].serialNumber, packet.gifts[i].giftName,
            packet.gifts[i].price, packet.gifts[i].storeName);
        fwrite(buffer_out, sizeof(char), strlen(buffer_out), file);
    }

    sprintf(buffer_out, "Total Cost: %.2f\n", packet.totalCost);
    fwrite(buffer_out, sizeof(char), strlen(buffer_out), file);

    // Freeing memory and closing file
    free(extension);
    fclose(file);
}