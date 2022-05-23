Group D
Haidar Musaqlab, John Miller, Shawn Kester, Michael Royster
Date: 3/25/2021

Dependencies: main program must be run on Linux system
			  that supports gnome-terminal
			  Developed in Ubuntu 20.04

Server:
Runs on separate machine (CSX)
	Compile: gcc -o server Server.c -pthread
	Run: ./server

Main:
Runs on local machine (requires gnome-terminal)

	Compile: make -B main
	Run: ./main

	Clear compiled objects: make clean


Code Contributions:
	Group Files: Main.c, Client.c
	Shawn Kester: Manager.c
	Haidar Musaqlab: Server.c
	John Miller: History.c
	Michael Royster: Assistant.c, Output.c
