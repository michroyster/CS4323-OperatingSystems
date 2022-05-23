#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "countPlayers.h"
#include "readPlayersInformation.h"

// Prints all previous players who have played the game

void displayGameSummary()
{
    // Get number of players in file
    int count = 0;
    int *pCount = &count;
    countPlayers(pCount);

    // Allocate memory for dynamic array of players
    Player *players = malloc(sizeof(struct Player) * count);

    // Populate array of players from file
    readPlayersInformation(players, count);

    // Print players
    printf("History of players: \n");
    int i;
    for (i = 0; i < count; i++)
    {
        printf("%d: %s, %s, %d\n", i + 1, (players + i)->name, (players + i)->date, (players + i)->totalGenerations);
    }

    // Free memory
    free(players);
}