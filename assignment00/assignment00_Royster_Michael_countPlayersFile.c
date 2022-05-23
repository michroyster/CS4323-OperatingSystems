#include <stdio.h>
#include <stdlib.h>
#include "Player.h"

// Takes a pointer to an int, and updates it to the 
// number of players in the file "players.txt"

void countPlayers(int *count)
{
    FILE *file;
    struct Player input;

    file = fopen("players.txt", "r");
    if (file == NULL)
    {
        printf("File reading error");
    }

    while(fread(&input, sizeof(struct Player), 1, file))
        *count = (*count) + 1;

    fclose(file);
}