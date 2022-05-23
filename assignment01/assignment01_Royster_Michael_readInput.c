#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "assignment01_Royster_Michael_Gift.h"

// Accepts a pointer to an array of Gift structs and loads it with
// all Gifts from the file "items.txt"

void readInput(Gift *gifts){
    FILE *file = fopen("items.txt", "r");
    if (file == NULL){
        printf("Unable to open file.");
        exit(1);
    }

    int length;
    char buffer[512];
    char *token;
    char temp[128];
    fgets(buffer, sizeof(buffer), file);
    length = strlen(buffer);
    if (length > 0 && buffer[length-1] == '\n')
        buffer[length-1] == '\0';

    int i = 0;
    char serialNumber[8];
    char price[32];
    while (!feof(file)){
        token = strtok(buffer,"\t");
        strcpy(serialNumber, token);

        token = strtok(NULL, "\t");
        strcpy((gifts + i)->giftName, token);

        token = strtok(NULL, " at ");
        strcpy(price, token);

        token = strtok(NULL, "\t");
        strcpy(temp, token);

        token = strtok(temp, "\n");
        strcpy((gifts + i)->storeName, token);

        token = strtok(serialNumber, ".");
        if (token != NULL)
            (gifts + i)->serialNumber = atoi(token);
        token = strtok(price, "$");
        if (token != NULL)
            (gifts + i)->price = atof(token);

        fgets(buffer, sizeof(buffer), file);
        length = strlen(buffer);
        if (length > 0 && buffer[length-1] == '\n'){
            buffer[length-1] == '\0';
        }
        i++;
    }

    fclose(file);
}

