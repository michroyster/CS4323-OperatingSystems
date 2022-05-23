#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "DynamicArray.h"

// Takes a pointer to a DynamicArray and Initializes board information in place

void boardInitialization(DynamicArray *array)
{
    FILE *file = fopen("state.txt", "r");
    if (file == NULL){
        printf("Unable to open file");
        exit(1);
    }

    char buffer[200];
    fgets(buffer, sizeof(buffer), file);
    
    // Get Dimensions
    sscanf(buffer, "%d %d", &array->y, &array->x);

    // Allocate Memory for 2D array
    array->board = (char *)malloc(sizeof(char) * array->y * array->x);

    char *token;
    char *temp = malloc(sizeof(char) * array->x);
    int i = 0;
    while(!feof(file))
    {
        fgets(buffer, sizeof(buffer), file);
        token = strtok(buffer, " ");

        strcpy(temp, token);
        while (token != NULL)
        {
            token = strtok(NULL, " ");
            if (token != NULL){

                strcat(temp, token);
            }
        }
        strcpy((array->board + i * array->x), temp);
        i++;
    }
    
    free(temp);
    fclose(file);
}