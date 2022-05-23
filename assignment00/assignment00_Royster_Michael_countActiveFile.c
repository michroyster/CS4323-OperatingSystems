#include <stdio.h>
#include <stdlib.h>
#include "DynamicArray.h"

// Takes in the cooridinaes of a position in a DynamicArray
// Returns the number of active cells around it.

int countActive(int y, int x, DynamicArray *array)
{
    int neighbors = 0;
    int above = 0;
    int below = 0;
    int left = 0;
    int right = 0;
    int offset = array->y * y + x;

    // Checking for edge cases
    if (y * array->y - array->y >= 0) above = 1;
    if (y < array->y) below = 1;
    if (x - 1 >= 0) left = 1;
    if (x + 1 < array->x) right = 1;

    // Counting active neighbors if they exist
    if (above)
    {
        if (*(array->board - array->y + offset) == 'X') neighbors++;
        if (left)
        {
            if(*(array->board - array->y - 1 + offset) == 'X') neighbors++;
        }
        if (right)
        {
            if (*(array->board - array->y + 1 + offset) == 'X') neighbors++;
        }
    }
    if (left)
    {
        if (*(array->board - 1 + offset) == 'X') neighbors++;

    }
    if (right)
    {
        if (*(array->board + 1 + offset) == 'X') neighbors++;
    }
    if (below)
    {
        if (*(array->board + array->y + offset) == 'X') neighbors++;
        if (left)
        {
            if (*(array->board + array->y - 1 + offset) == 'X') neighbors++;
        }
        if (right)
        {
            if (*(array->board + array->y + 1 + offset) == 'X') neighbors++;
        }
    }
    return neighbors;
}