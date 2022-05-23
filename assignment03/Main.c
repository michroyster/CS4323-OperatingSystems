#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include "Reservation.h"
#include "Backend.h"
#include "Server.h"

#define PORT 8019
#define SERVER_COUNT 5

int main(){  
    // Create semaphores and shared memory object
    sem_t *file_write;
    sem_t *file_read;
    int shm_fd;
    int *ptrReaders;
    init_sync(file_write, file_read, shm_fd, ptrReaders);

    // create a bunch of processes to read at the same time
    char name = 'A';
    int parentid = getpid();
    for (int i = 0; i < SERVER_COUNT; i++){
        if (fork() == 0){
            Server(name + i, PORT + i);
            return 0;
        }
    }

    for(int j = 0; j < 5; j++){
        wait(NULL);
    }

    // desync
    desync(file_write, file_read, shm_fd, ptrReaders);

    return 0;
}