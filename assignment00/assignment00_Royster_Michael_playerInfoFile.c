#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "playerInfo.h"

// Takes a pointer to a Player
// Collects information from the player

void playerInfo(Player *player)
{
    char name[64];
    char date[64];
    char gen[64];
    printf("Enter player name: ");
    fgets(name, 64, stdin);
    printf("Enter date: ");
    fgets(date, 64, stdin);
    printf("Enter number of generations: ");
    fgets(gen, 64, stdin);

    strtok(name, "\n");
    strtok(date, "\n");

    strcpy(player->name, name);
    strcpy(player->date, date);
    player->totalGenerations = atoi(gen);
}