// Author: Shawn Kester
// Group D
// Email: smkeste@okstate.edu

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
#include "Client.h"
#include "Query.h"


int Manager(){
    char employee_name[50];
    char job_title[64];
    char status[3];
    char x[2];
    int fd;
    char *check = "exit";

    char myPipe[] = "./ManagerPipe";
    mkfifo(myPipe, 0666);

    if ((fd = open(myPipe, O_WRONLY | O_CREAT)) < 0)
        printf("Manager: Error Opening..");

    do{
        printf("Enter the Employee Name: ");
        fgets(employee_name, sizeof(char) * 50, stdin);
        strtok(employee_name, "\n");

        printf("Enter Job Title: ");
        fgets(job_title, sizeof(char) * 64, stdin);
        strtok(job_title, "\n");

        printf("Enter Status: ");
        fgets(status, sizeof(char) * 10, stdin);
        strtok(status, "\n");

        Query query;
        strcpy(query.employee_name, employee_name);
        strcpy(query.job_title, job_title);
        strcpy(query.status, status);
        Query *ptr_query = &query;
        
        if (write(fd, &query, sizeof(Query)) < 0)
            printf("Manager: Error writing...");

        printf("Do you want to look for another entry? \n Y/N \n");//ask to continue input taking
		fgets(x, sizeof(char) * 10, stdin);
        strtok(x, "\n");

    } while (x[0] == 'y' || x[0] =='Y');

    Query exit;
    strcpy(exit.employee_name, "exit");
    strcpy(exit.job_title, "exit");
    strcpy(exit.status, "ex");

    if (write(fd, &exit, sizeof(Query)) < 0)
        printf ("Manager: error sending exit message");

    close(fd);

    return 0;
}