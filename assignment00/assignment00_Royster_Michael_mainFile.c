#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

/*
    Author Name: Michael Royster
    Email: micaher@okstate.edu
    Date: 2/1/2021
    Program Description: This program loads an N x M dimension board into a dynamic 2D array
    that is handled by pointer arithmetic. Each position in the array will be an X or -.
    The positions will be flipped to X's or -'s for a user input amount of generations based
    on specifc rules about their neighboring positions. At the end the player's info, date,
    and number of generations played will be written to a text file.
*/

int main()
{
    DynamicArray dynamicArray; // Declare Struct  
    DynamicArray *ptrArray = &dynamicArray; // Declare Array and set it to dynamicArray address

    // Read into file
    boardInitialization(ptrArray);

    // Get player info
    Player player = {"temp", "temp", 0};
    Player *ptrPlayer = &player;
    playerInfo(ptrPlayer);

    // Variables for counting total generations played
    int totalGen = 0;
    int *pTotalGen = &totalGen;

    // Display generations including initial state
    displayGeneration(ptrPlayer->totalGenerations, ptrArray, ptrPlayer, pTotalGen);
    
    // Used for getting input from the player
    char response[8];
    int moreGenerations = 0;
    
    // Ask if player wants more generations
    do{
        printf("\nWould you like to display additional generations? ");
        scanf("%s", response);
        if((*response == 'y' || *response == 'Y'))
        {
            printf("Enter number of generations: ");
            scanf("%d", &moreGenerations);
            displayGeneration(moreGenerations, ptrArray, ptrPlayer, pTotalGen);
        }
    } while ((*response == 'y' || *response == 'Y') && !gameOverCheck(ptrArray));

    // Updating the player's total generations played
    ptrPlayer->totalGenerations = *pTotalGen;
    
    // Writing player info to file
    writeFile(ptrPlayer);

    // Printing all players who have played the game
    displayGameSummary();

    // Freeing memory before exit
    free(ptrArray->board);
    return 0;
}