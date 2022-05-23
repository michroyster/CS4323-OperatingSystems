Author Name: Michael Royster
Email: micaher@okstate.edu
Date: 2/14/2021
Program Description: assignment01
Use POSIX IPC mechanisms in Unix: Memory Sharing and Message Sending.
Server(main) Process loads items.txt into memory and creates shared memroy.
Server Process forks N Customer Processes and 1 Helper Process.
Each Customer Process asks for M random gifts selected from Shared Memory Space.
Each Customer Process then sends the gifts via Message Queue to Helper Process.
Helper Process asks for the order in which to process the Customers.
Helper then calculates total cost, prints receipt on screen in order
and saves each receipt to a file.
Server Process prints "Thank you" when Helper Process finishes.


********* Instructions *********
*    Compile: make -B main     *
*    Run: ./main               *
********************************


To remove all compiled files and receipts: make clean