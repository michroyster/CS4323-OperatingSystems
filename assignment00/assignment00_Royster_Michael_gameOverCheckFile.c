#include <stdio.h>
#include <stdlib.h>
#include "DynamicArray.h"

// Takes a pointer to a DynamicArray
// Returns 1 if game is over, and 0 if game is still active

int gameOverCheck(DynamicArray *array)
{
    int i, j;
    for (i = 0; i <  array->y; i++){
        for (j = 0; j < array->x; j++) {
            if(*(array->board + i * array->y + j) == 'X')
                return 0;
        }
    }
    return 1;
}