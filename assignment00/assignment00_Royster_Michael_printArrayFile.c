#include <stdio.h>
#include "DynamicArray.h"

// Takes a pointer to a DynamicArray
// Prints the array

void printArray(DynamicArray *array)
{
    int i, j;
    for (i = 0; i <  array->y; i++){
        for (j = 0; j < array->x; j++) {
            printf("%c ", *(array->board + i * array->y + j));
        }
        printf("\n");
    }
}