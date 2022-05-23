#include <stdio.h>
#include <stdlib.h>
#include "Player.h"

// Takes a pointer to an array of Players and an int
// Populates the array with the players info from players.txt

void readPlayersInformation(Player *players, int count)
{
    FILE *file;
    struct Player input;

    file = fopen("players.txt", "r");
    if (file == NULL)
    {
        printf("File reading error");
    }

    int i;
    for (i = 0; i < count; i++)
    {
        fread(&input, sizeof(struct Player), 1, file);
        *(players + i) = input;
    }

    fclose(file);
}