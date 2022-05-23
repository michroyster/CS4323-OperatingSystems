#include <stdio.h>
#include <stdlib.h>
#include "DynamicArray.h"
#include "countActive.h"
#include "copyArray.h"

// Takes a pointer to a DynamicArray
// Updates the array to the next generation based on the rules.

void generations(DynamicArray *array)
{
    // DynamicArray *temp = malloc(sizeof(array)); // For CSX
    DynamicArray temporary;
    DynamicArray *temp = &temporary; // For my machine
    copyArray(temp, array);
    int activeCount = 0;
    int active = 0;
    int i, j;

    for (i = 0; i <  array->y; i++){
        for (j = 0; j < array->x; j++) {
            activeCount = countActive(i, j, array);
            if (*(array->board + i * array->y + j) == 'X'){
                // Rule 1 and 2.
                if(activeCount < 2 || activeCount > 3)
                {
                    *(temp->board + i * temp->y + j) = '-';
                }
                // Rule 3.
                else if (activeCount == 2 || activeCount == 3)
                {
                    *(temp->board + i * temp->y + j) = 'X';
                }
            }
            else
            {
                // Rule 4.
                if (activeCount == 3)
                {
                    *(temp->board + i * temp->y + j) = 'X';
                }
            }
        }
    }
    copyArray(array, temp);
    free(temp->board);
}