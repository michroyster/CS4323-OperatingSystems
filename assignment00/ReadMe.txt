Author Name: Michael Royster
Email: micaher@okstate.edu
Date: 2/1/2021
Program Description: This program loads an N x M dimension board into a dynamic 2D array
that is handled by pointer arithmetic. Each position in the array will be an X or -.
The positions will be flipped to X's or -'s for a user input amount of generations based
on specifc rules about their neighboring positions. At the end the player's info, date,
and number of generations played will be written to a text file, "players.txt".


*************************************************
Preferred Method to Run program:                *
    Instructions to run with Makefile:          *
        To compile: make main                   *
        To run: ./main                          *
*************************************************


Alternative method to run program:
    Run the following commands to compile:
            gcc -c -o main.o assignment00_Royster_Michael_mainFile.c
            gcc -c -o playerInfo.o assignment00_Royster_Michael_playerInfoFile.c
            gcc -c -o boardInitialization.o assignment00_Royster_Michael_boardInitializationFile.c
            gcc -c -o generations.o assignment00_Royster_Michael_generationsFile.c
            gcc -c -o displayGeneration.o assignment00_Royster_Michael_displayGenerationFile.c
            gcc -c -o readPlayersInformation.o assignment00_Royster_Michael_readPlayersInformationFile.c
            gcc -c -o displayGameSummary.o assignment00_Royster_Michael_displayGameSummaryFile.c
            gcc -c -o printArray.o assignment00_Royster_Michael_printArrayFile.c
            gcc -c -o countPlayers.o assignment00_Royster_Michael_countPlayersFile.c
            gcc -c -o writeFile.o assignment00_Royster_Michael_writeFileFile.c
            gcc -c -o copyArray.o assignment00_Royster_Michael_copyArrayFile.c
            gcc -c -o countActive.o assignment00_Royster_Michael_countActiveFile.c
            gcc -c -o gameOverCheck.o assignment00_Royster_Michael_gameOverCheckFile.c
            gcc -c -o writeFile.o assignment00_Royster_Michael_writeFileFile.c
            gcc -o main main.o playerInfo.o boardInitialization.o generations.o displayGeneration.o readPlayersInformation.o displayGameSummary.o printArray.o copyArray.o writeFile.o countActive.o gameOverCheck.o countPlayers.o

    To run: ./main