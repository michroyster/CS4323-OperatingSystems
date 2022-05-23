// Author: Group D

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/msg.h>
#include <errno.h>
#include <mqueue.h>
#include "Assistant.h"
#include "Manager.h"
#include "Client.h"
#include "Query.h"

int Client(){

    if(fork() == 0){ // Manager
        Manager();
    }else{           // Assistant
        Assistant();
        wait(NULL);
        wait(NULL);
    }

    return 0;
}