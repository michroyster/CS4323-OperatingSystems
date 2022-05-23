// Author: Michael Royster
// Group D
// Email: micaher@okstate.edu

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
#include "Record.h"
#include "Query.h"

int main(){
    printf("Output: \n");

    int fd;
    Query query;
    Record record;

    char assistant_pipe[] = "./AssistantPipe";
    mkfifo(assistant_pipe, 0666);

    if ((fd = open(assistant_pipe, O_RDONLY)) < 0)
        perror("Output: error opening...");
    
    while(1){
        bzero(&record, sizeof(Record));

        if (read(fd, &record, sizeof(Record)) < 0)
            perror("Output: error reading...");
        
        // Terminate when told
        if(strcmp(record.employee_name, "exit") == 0)
            break;

        // printf("Name: %s, Title: %s, Status: %s\n", query.employee_name, query.job_title, query.status);
        printf("%d %s %s %.2f %.2f %.2f %s %.2f %d %d %d %d %d\n",
                record.id,
                record.employee_name,
                record.job_title,
                record.base_pay,
                record.overtime_pay,
                record.benefit,
                record.status,
                record.satisfaction_level,
                record.number_project,
                record.average_monthly_hours, 
                record.time_spend_company_in_yrs,
                record.work_accident,
                record.promotion_last_5years);
    }

    close(fd);
    return 0;
}