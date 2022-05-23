#include <stdio.h>
#include <stdlib.h>
#include "DynamicArray.h"
#include "Player.h"
#include "printArray.h"
#include "generations.h"
#include "gameOverCheck.h"

// Takes an integer, and a pointer to a DynamicArray, Player and int
// Displays the number of generations player requests. Ends if game is over.

void displayGeneration(int numGenerations, DynamicArray *array, Player *player, int *totalGen)
{
    int activeBoard = 0;

    printf("Initial Generation:\n");
    printArray(array);
    for(int i = 1; i <= numGenerations && activeBoard == 0; i++)
    {
        *totalGen = (*totalGen) + 1;
        printf("Generation: %d\n", *totalGen);
        generations(array);
        printArray(array);
        if(activeBoard = gameOverCheck(array))
            printf("\nAfter generation %d, all cells in the board are inactive!\n\n", i);
    }
}