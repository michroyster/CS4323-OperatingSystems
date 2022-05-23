#include <stdio.h>
#include <stdlib.h>
#include "Player.h"

// Takes pointer to a Player
// Appends the player info to players.txt

void writeFile(Player *player)
{
    FILE *file = fopen("players.txt", "a");
    if (file == NULL)
    {
        printf("fopen file error");
    }

    fwrite(player, sizeof(struct Player), 1, file);

    if (fwrite == 0)
        printf("error writing to file\n");

    fclose(file);
}