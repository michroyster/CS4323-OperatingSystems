Authors:
    Group E
    Jareth Harmon
    Michael Royster
    Bill Liando

Details:
    -Client interacts with server by passing a buffer back and forth.
    -Multiple clients can connect to the server but only one client can make reservations at a time to reduce any double booking.
    -Train Car can handle 27 seats on each day.
    -The assignment specifies that the client MUST get the seat after confirming their reservation. An assumption was made 
     that the only way to do that is to only allow 1 client to interact with the seat list at a time 
     (for making reservations/modifying them). Otherwise by the time the client leaves the priority queue after confirming,
     there might not be enough seats left for them.
    
Compile:
    make -B main
    make -B client

Run Server:
    ./main

Run Client:
    ./client

Clean object files:
    make clean

Remove named semaphores:
    make remove