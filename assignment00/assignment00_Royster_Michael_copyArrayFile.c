#include <stdio.h>
#include <stdlib.h>
#include "DynamicArray.h"

// Takes in two pointers, and copies one to another by pointer

void copyArray(DynamicArray *dest, DynamicArray *origin)
{
    dest->y = origin->y;
    dest->x = origin->x;

    dest->board = (char *)malloc(sizeof(char) * dest->y * dest->x);

    int i, j;
    for (i = 0; i < origin->y; i++){
        for (j = 0; j < origin->x; j++) {
            *(dest->board + i * dest->y + j) = *(origin->board + i * origin->y + j);
        }
    }
}