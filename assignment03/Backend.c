// Author: Michael Royster
// Email: micaher@okstate.edu

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "Reservation.h"
#include "Backend.h"

#define MAX_SEATS 27
#define BUFFER_SIZE 2048

// Initialize semaphores and shared memory space
void init_sync(sem_t *file_write, sem_t *file_read, int shm_fd, int *ptrReaders){
    // Create semaphores
    file_write = sem_open("/file_write", O_CREAT, 0666, 1);
    file_read = sem_open("/file_read", O_CREAT, 0666, 1);

    // Create shared memory object
    shm_fd = shm_open("readers", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(int));
    ptrReaders = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *ptrReaders = 0;
}

// Clean up semaphores and shared memory space
void desync(sem_t *file_write, sem_t *file_read, int shm_fd, int *ptrReaders){
    // unmap, close and delete shared memory object
    munmap(ptrReaders, sizeof(int));
    close(shm_fd);
    shm_unlink("readers");

    // Unlink and close semaphores
    sem_unlink("/file_write");
    sem_close(file_write);
    sem_unlink("/file_read");
    sem_close(file_read);
}

// Testing purposes only
void ServerX(char name){
    char server_name = name; // this should later be a parameter of Server(char name)

    // Reservation *info = (Reservation*)malloc(sizeof(Reservation)*27);
    // inquiry("4172021-2", info);
    // printf("%c\n", server_name);
    // for (int i =0 ;i < 3; i++){
    //     printf("%s\n",(info+i)->customerName);
    // }
    // free(info);

    Reservation reservation1 = {"Katy", "1-1-1900", "M", 12345, "4172021", 2, "D2"};
    Reservation reservation2 = {"Joey", "1-1-2122", "M", 45768, "4772021", 2, "A3"};

    // Reservation reservation3 = {"Steve", "1-1-2122", "M", 45768, "4172021", 2, "A3"};
    // Reservation reservation4 = {"Johny", "1-1-2122", "M", 45768, "4172021", 2, "A4"};
    Reservation *reservations = (Reservation*)malloc(sizeof(Reservation) * 4);
    // *reservations = reservation3;
    // *(reservations+1) = reservation4;

    // Reservation *reservations = (Reservation*)malloc(sizeof(Reservation) * 2);
    *reservations = reservation1;
    *(reservations+1) = reservation2;
    // add_travelers(server_name, reservations, 2, "4172021-1");
    // inquiry("4172021-1", reservations);
    // printf("1: %s \n2: %s\n3: %s\n4: %s\n", reservations->customerName, (reservations+1)->customerName, (reservations+2)->customerName, (reservations+3)->customerName);
    // make_reservation(server_name, reservations, 2);
    // update_train_seats("4172021-3", "Sean", "B3", server_name);
    // remove_traveler("4172021-1", "Joey");
    // cancel_reservation("4172021-4");
    // char arr[112];
    // available_seats(4172021, arr);
    // printf("%s\n", arr);
    // check_seat(4172021, "A3");
    // receipt(reservations, 2, name);
}

// Get date for today and tomorrow
void get_date(char* today, char* tomorrow){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(today, sizeof(char)*10, "%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    snprintf(tomorrow, sizeof(char)*10, "%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday+1);
}

// return 1 if filename is in directory
int file_exists(char *filename){
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d){
        while ((dir =readdir(d)) != NULL){
            if (strcmp(filename, dir->d_name) == 0){
                closedir(d);
                return 1;
            }
        }
    }
    closedir(d);
    return 0;
}

// Has a critical section - WRITE, threadsafe X
// Updated 4/23, Now takes an array of passengers and the number of passengers
// Ticket number is based on the travelDate of the first reservation 
void make_reservation(char server, Reservation* reservation, int numberTravelers){  
    //time_t t = time(NULL);
    //struct tm tm = *localtime(&t);
   // char today[9];
   // snprintf(today, sizeof(char) * 8, "%d", tm.tm_mday);
	char date[32];
	strcpy(date, reservation->travelDate);
   // int date = atoi(reservation->travelDate);
    // char filename[24];
    // sprintf(filename, "%d.txt", date);
    char* filename = "summary.txt";
    char buffer[BUFFER_SIZE];
    int tick = 1;
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_wait(file_write);

    if (file_exists(filename)){
        file = fopen(filename, "r");
		int tmp = 0;
		char *rest = "";
        while(fgets(buffer, sizeof(buffer), file)){
			char *tok = strtok_r(buffer, " \t", &rest);
			
			if (atoi(tok) > tmp) tmp = atoi(tok);
			
			tick++;
			memset(buffer, 0, sizeof(buffer));
			memset(tok, 0, sizeof(tok));
        }
		tick = tmp + 1;
        fclose(file);
    }
    
    char ticket[24];
	sprintf(ticket, "%d", tick);
	//sprintf(ticket, "%s-%d", date, tick);
    //sprintf(ticket, "%d-%d", date, tick);
    for (int j = 0; j < numberTravelers; j++)
        strcpy((reservation+j)->ticket_number, ticket);
    file = fopen(filename, "a");
    char buffer_out[BUFFER_SIZE];
    for (int i = 0; i < numberTravelers; i++){
        sprintf(buffer_out, "%s\t%c\t%s\t%s\t%s\t%d\t%s\t%s\tOG\n", ticket, server, (reservation+i)->customerName, (reservation+i)->dob, (reservation+i)->gender, (reservation+i)->govID, (reservation+i)->travelDate, (reservation+i)->seat);
        fprintf(file, "%s", buffer_out);
		//puts((reservation+i)->dob);
    }
    fclose(file);
    sem_post(file_write);

   // receipt(reservation, numberTravelers, server, "");
}

// Has critical section - Write, threadsafe X
// Adds travelers with the given ticket number
void add_travelers(char server, Reservation* reservation, int numberNewTravelers, char* ticket){
    // time_t t = time(NULL);
    // struct tm tm = *localtime(&t);
    // char today[9];
    // snprintf(today, sizeof(char) * 8, "%d%d%d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
    int date = atoi(reservation->travelDate);
    // char filename[24];
    // sprintf(filename, "%d.txt", date);
    char* filename = "summary.txt";
    char buffer[BUFFER_SIZE];
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_wait(file_write);

    for (int j = 0; j < numberNewTravelers; j++)
        strcpy((reservation+j)->ticket_number, ticket);
    file = fopen(filename, "a");
    char buffer_out[BUFFER_SIZE];
    for (int i = 0; i < numberNewTravelers; i++){
        sprintf(buffer_out, "%s\t%c\t%s\t%s\t%s\t%d\t%s\t%s\tOG\n", ticket, server, (reservation+i)->customerName, (reservation+i)->dob, (reservation+i)->gender, (reservation+i)->govID, (reservation+i)->travelDate, (reservation+i)->seat);
        fprintf(file, "%s", buffer_out);
    }
    fclose(file);
    sem_post(file_write);
}

// Has critical section - READ, threadsafe X
// Updated 4/22, Now takes ticket number and array of reservations and places all 
// reservations that match the ticket number are placed into the array (in place in memory)
void inquiry(char *ticket, Reservation* info, int* numTravelers){
    // find ticket
    char date[9];
    for (int i = 0; i < 7; i++) *(date+i) = *(ticket+i);
    // char filename[16];
    // sprintf(filename, "%s.txt", date);
    char* filename = "summary.txt";

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_t *file_read = sem_open("/file_read", O_CREAT, 0666, 0);

    // first reader
    sem_wait(file_read);
    int shm_fd = shm_open("readers", O_RDWR, 0666);
    int *reader_count;
    reader_count = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *reader_count = *reader_count + 1;

    if (*reader_count == 1){
        sem_wait(file_write);
    }
    sem_post(file_read);
    Reservation resy;
    int count = 0;
    if (file_exists(filename)){
        char buffer[512];
        char temp[512];
        char *rest = buffer;
        char *token;
        int flag = 0;
        FILE *file = fopen(filename, "r");
        fgets(buffer, sizeof(buffer), file);
        while(!feof(file)){
            strcpy(temp, buffer);
            token = strtok_r(temp, "\t", &rest);  //Ticket
            if (strcmp(ticket, token) == 0){
                flag = 1;
                strcpy(resy.ticket_number, ticket);
                token = strtok_r(NULL, "\t", &rest); // Server
                token = strtok_r(NULL, "\t", &rest); // Name
                strcpy(resy.customerName, token);
                token = strtok_r(NULL, "\t", &rest); // dob
                strcpy(resy.dob, token);
                token = strtok_r(NULL, "\t", &rest); // gender
                strcpy(resy.gender, token);
                token = strtok_r(NULL, "\t", &rest); // govid
                resy.govID = atoi(token);
                token = strtok_r(NULL, "\t", &rest); // traveldate
                strcpy(resy.travelDate, token);
                token = strtok_r(NULL, "\t", &rest); // seat
                strcpy(resy.seat, token);

                *(info + count) = resy;
                count++;
            }
            fgets(buffer, sizeof(buffer), file);
        }
        if (!flag) {
            printf("Ticket not found.\n");
        }
        fclose(file);
    }else {
        printf("Ticket not found!");
    }
    
    *numTravelers = count;

    // last reader
    sem_wait(file_read);
    *reader_count = *reader_count - 1;
    if (*reader_count == 0){
        sem_post(file_write);
    }
    sem_post(file_read);
    munmap(reader_count, sizeof(int));
    close(shm_fd);
}

// Has critical section - WRITE, threadsafe X
// Updated 4/23, Now requires both ticket number and name
void update_train_seats(char* ticket, char *name, char* seat, char server){
    // create filename from date
    char date[9];
    for (int i = 0; i < 7; i++) *(date+i) = *(ticket+i);
    // char filename[16];
    // sprintf(filename, "%s.txt", date);
    char* filename = "summary.txt";

    char buffer_in[MAX_SEATS][BUFFER_SIZE];
    char line[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    char *rest = temp;
    char buffer_out[BUFFER_SIZE];
    char *token;
    int count = 0;
    int flag = 0;
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_wait(file_write);

    if (file_exists(filename)){
        file = fopen(filename, "r");
        fgets(line, sizeof(line), file);
        while(!feof(file)){
            strcpy(temp, line);
            token = strtok_r(temp, "\t", &rest);
            if (strcmp(token, ticket) == 0){
                strcpy(buffer_out, token);
                strcat(buffer_out, "\t");
                token = strtok_r(NULL, "\t", &rest);
                strcat(buffer_out, (char[2]){(char) server, '\0'}); // casting char server name as string for strcat
                strcat(buffer_out, "\t");
                token = strtok_r(NULL, "\t", &rest);
                if (strcmp(token, name) == 0){
                    flag = 1;
                    strcat(buffer_out, token);
                    strcat(buffer_out, "\t");
                    for (int i = 0; i < 4; i++){
                        token = strtok_r(NULL, "\t", &rest);
                        strcat(buffer_out, token);
                        strcat(buffer_out, "\t");
                    }
                    strcat(buffer_out, seat);
                    strcat(buffer_out, "\tMD");
                    strcat(buffer_out, "\n");
                    strcpy(buffer_in[count], buffer_out);
                } else{
                    strcpy(buffer_in[count], line);
                }
            }else{
                strcpy(buffer_in[count], line);
            }
            fgets(line, sizeof(buffer_in), file);
            count++;
        }
        fclose(file);
        if (!flag) {
            printf("Ticket not found!\n");
        }
        file = fopen(filename, "w");
        for (int i = 0; i < count; i++){
            fprintf(file, "%s", buffer_in[i]);
        }
        fclose(file);
    }else{
        printf("Ticket not found!\n");
    }
    sem_post(file_write);
}

// Has critical section - WRITE, threadsafe X
// Updated 4/25, cancel by ticket number only
void cancel_reservation(char* ticket){
    // create filename from date
    char date[9];
    for (int i = 0; i < 7; i++) *(date+i) = *(ticket+i);
    // char filename[16];
    // sprintf(filename, "%s.txt", date);
    char* filename = "summary.txt";

    char buffer_in[MAX_SEATS][BUFFER_SIZE];
    char line[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    char *rest = temp;
    char *token;
    int count = 0;
    int flag = 0;
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_wait(file_write);

    if (file_exists(filename)){
        file = fopen(filename, "r");
        fgets(line, sizeof(line), file);
        while(!feof(file)){
            strcpy(temp, line);
            token = strtok_r(temp, "\t", &rest);
            if (strcmp(token, ticket) == 0){
                flag = 1;
                count--;
            }else{
                strcpy(buffer_in[count], line);
            }
            fgets(line, sizeof(buffer_in), file);
            count++;
        }
        if (!flag) printf("Ticket not found!\n");
        fclose(file);

        file = fopen(filename, "w");
        for (int i = 0; i < count; i++){
            fprintf(file, "%s", buffer_in[i]);
        }
        fclose(file);
    }else{
        printf("Ticket not found!\n");
    }
    sem_post(file_write);
}

// Has critical section - WRITE, threadsafe X
// Cancel by ticket number and name
void remove_traveler(char* ticket, char* name){
    // create filename from date
    char date[9];
    for (int i = 0; i < 7; i++) *(date+i) = *(ticket+i);
    // char filename[16];
    // sprintf(filename, "%s.txt", date);
    char* filename = "summary.txt";

    char buffer_in[MAX_SEATS][BUFFER_SIZE];
    char line[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    char *rest = temp;
    char *token;
    int count = 0;
    int flag = 0;
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_wait(file_write);

    if (file_exists(filename)){
        file = fopen(filename, "r");
        fgets(line, sizeof(line), file);
        while(!feof(file)){
            strcpy(temp, line);
            token = strtok_r(temp, "\t", &rest);
            if (strcmp(token, ticket) == 0){
                token = strtok_r(NULL, "\t", &rest);
                token = strtok_r(NULL, "\t", &rest);
                if (strcmp(token, name) == 0){
                    flag = 1;
                    count--;
                }else{
                    strcpy(buffer_in[count], line);
                }
            }else{
                strcpy(buffer_in[count], line);
            }
            fgets(line, sizeof(buffer_in), file);
            count++;
        }
        if (!flag) printf("Ticket not found!\n");
        fclose(file);

        file = fopen(filename, "w");
        for (int i = 0; i < count; i++){
            fprintf(file, "%s", buffer_in[i]);
        }
        fclose(file);
    }else{
        printf("Ticket not found!\n");
    }
    sem_post(file_write);
}

// Has critical section - READ, threadafe X
// Puts a string of the available seats into options delimited by spaces
//void available_seats(int date, char* options, int* numberAvailable){
void available_seats(char *date, char* options, int* numberAvailable){
    char all_seats[3][9][4];
    char x[4];
    char first = 'A';
    int num = 1;
    for (int i = 0; i < 3; i ++){
        for (int j = 0; j < 9; j++){
            sprintf(x, "%c%d", first + i, num + j);
            strcpy(all_seats[i][j], x);
        }
    }
    printf("\n");
    char taken[MAX_SEATS][3];
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    char *rest = temp;
    char available[100];
    char *token;
    int count = 0;
    // char filename[16];
    // sprintf(filename, "%d.txt", date);
    char* filename = "summary.txt";
    FILE *file;

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_t *file_read = sem_open("/file_read", O_CREAT, 0666, 0);

    // first reader
    sem_wait(file_read);
    int shm_fd = shm_open("readers", O_RDWR, 0666);
    int *reader_count;
    reader_count = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *reader_count = *reader_count + 1;

    if (*reader_count == 1){
        sem_wait(file_write);
    }
    sem_post(file_read);

    if (file_exists(filename)){
        file = fopen(filename, "r");
        fgets(buffer, sizeof(buffer), file);
        strcpy(temp, buffer);
        while(!feof(file)){
            token = strtok_r(temp, "\t", &rest);
            for (int i = 0; i < 6; i++) token = strtok_r(NULL, "\t", &rest);
            if (!strcmp(token, date)){
			//if(atoi(token) == date){
                token = strtok_r(NULL, "\t", &rest);
                strcpy(taken[count], token);
            }
            fgets(buffer, sizeof(buffer), file);
            strcpy(temp, buffer);
            count++;
        }
    }else{
        printf("Ticket not found!\n");
    }
	int availSeats = 27;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 9; j++){
            for (int k = 0; k < count; k++){
                if (strcmp(all_seats[i][j], taken[k]) == 0){
                    strcpy(all_seats[i][j], "XX");
					availSeats--;
                }
            }
        }
    }
    char row[38];
    strcpy(options, "");
    for (int i = 0; i < 3; i ++){
        for (int j = 0; j < 9; j++){
            if (strcmp(all_seats[i][j], "XX") != 0){
                strcat(options, all_seats[i][j]);
                strcat(options, " ");
            }
			strcat(options, "\t");
        }
		strcat(options, "\n");
    }
    *numberAvailable = availSeats;
    // last reader
    sem_wait(file_read);
    *reader_count = *reader_count - 1;
    if (*reader_count == 0){
        sem_post(file_write);
    }
    sem_post(file_read);
    munmap(reader_count, sizeof(int));
    close(shm_fd);
}

// Has critical section by calling available_seats, threadsafe X
// Returns 1 if seat is available, and returns 0 if seat is taken
//int check_seat(int date, char *seat){
int check_seat(char *date, char *seat){
    char avail[112];
    int* temp;
    available_seats(date, avail, temp);
    char* s = strstr(avail, seat);
    if (s != NULL){
        printf("Found seat %s\n", seat);
        return 1;
    }else{
        printf("Seat %s not found\n", seat);
        return 0;
    }
}

void get_travel_date(char* ticket, char* travelDate){
    char* filename = "summary.txt";

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_t *file_read = sem_open("/file_read", O_CREAT, 0666, 0);

    // first reader
    sem_wait(file_read);
    int shm_fd = shm_open("readers", O_RDWR, 0666);
    int *reader_count;
    reader_count = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *reader_count = *reader_count + 1;

    if (*reader_count == 1){
        sem_wait(file_write);
    }
    sem_post(file_read);
    int count = 0;
    if (file_exists(filename)){
        char buffer[512];
        char temp[512];
        char *rest = buffer;
        char *token;
        int flag = 0;
        FILE *file = fopen(filename, "r");
        fgets(buffer, sizeof(buffer), file);
        while(!feof(file)){
            strcpy(temp, buffer);
            token = strtok_r(temp, "\t", &rest);  //Ticket
            if (strcmp(ticket, token) == 0){
                flag = 1;
                token = strtok_r(NULL, "\t", &rest); // Server
                token = strtok_r(NULL, "\t", &rest); // Name
                token = strtok_r(NULL, "\t", &rest); // dob
                token = strtok_r(NULL, "\t", &rest); // gender
                token = strtok_r(NULL, "\t", &rest); // govid
                token = strtok_r(NULL, "\t", &rest); // traveldate
                strcpy(travelDate, token);
                break;
                token = strtok_r(NULL, "\t", &rest); // seat
                count++;
            }
            fgets(buffer, sizeof(buffer), file);
        }
        if (!flag) {
            printf("Ticket not found.\n");
        }
        fclose(file);
    }else {
        printf("Ticket not found!");
    }

    // last reader
    sem_wait(file_read);
    *reader_count = *reader_count - 1;
    if (*reader_count == 0){
        sem_post(file_write);
    }
    sem_post(file_read);
    munmap(reader_count, sizeof(int));
    close(shm_fd);
}

// Has critical section - READ
void get_num_travelers(char* ticket, int* numTravelers){
    char* filename = "summary.txt";

    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_t *file_read = sem_open("/file_read", O_CREAT, 0666, 0);

    // first reader
    sem_wait(file_read);
    int shm_fd = shm_open("readers", O_RDWR, 0666);
    int *reader_count;
    reader_count = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *reader_count = *reader_count + 1;

    if (*reader_count == 1){
        sem_wait(file_write);
    }
    sem_post(file_read);
    int count = 0;
    if (file_exists(filename)){
        char buffer[512];
        char temp[512];
        char *rest = buffer;
        char *token;
        int flag = 0;
        FILE *file = fopen(filename, "r");
        fgets(buffer, sizeof(buffer), file);
        while(!feof(file)){
            strcpy(temp, buffer);
            token = strtok_r(temp, "\t", &rest);  //Ticket
            if (strcmp(ticket, token) == 0){
                flag = 1;
                count++;
            }
            fgets(buffer, sizeof(buffer), file);
        }
        if (!flag) {
            printf("Ticket not found.\n");
        }
        fclose(file);
    }else {
        printf("Ticket not found!");
    }
    *numTravelers = count;

    // last reader
    sem_wait(file_read);
    *reader_count = *reader_count - 1;
    if (*reader_count == 0){
        sem_post(file_write);
    }
    sem_post(file_read);
    munmap(reader_count, sizeof(int));
    close(shm_fd);
}


// Create receipt X
void receipt(Reservation* reservations, int numberTravelers, char server, char* receipt){
    char buffer[BUFFER_SIZE];
    char buffer_out[1024] = "";

    char filename[40];
    sprintf(filename, "Receipt-%s.txt", reservations->ticket_number);

    FILE *file = fopen(filename, "w");
    fprintf(file, "%s%s\n", "Receipt for ticket number: ", reservations->ticket_number);
    fprintf(file, "%s%c\n\n", "Server: ", server);
    for (int i = 0; i < numberTravelers; i++){
        sprintf(buffer, "%s\t%s\t%s\t%s\t%d\t%s\t%s\n", (reservations+i)->ticket_number, (reservations+i)->customerName, (reservations+i)->dob, (reservations+i)->gender, (reservations+i)->govID, (reservations+i)->travelDate, (reservations+i)->seat);
        fprintf(file, "%s", buffer);
    }
    fclose(file);
    file = fopen(filename, "r");
    char rec[1024] = "";
    while(!feof(file)){
		memset(buffer, 0, sizeof(buffer));
        fgets(buffer, BUFFER_SIZE, file);
        strcat(rec, buffer);
    }
    //for (int i = 0; i < 1024 || rec[i] == '\0'; i++){
	for (int i = 0; i < BUFFER_SIZE-24 || rec[i] == '\0'; i++){
        *(receipt+i) = *(rec+i);
    }
    fclose(file);


}

// testing purposes only
void testX(char name){
    sem_t *file_write = sem_open("/file_write", O_CREAT, 0666, 0);
    sem_t *file_read = sem_open("/file_read", O_CREAT, 0666, 0);
    
    // first reader
    sem_wait(file_read);
    int shm_fd = shm_open("readers", O_RDWR, 0666);
    int *reader_count;
    reader_count = mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *reader_count = *reader_count + 1;
    printf("%c entering: render_count: %d\n", name, *reader_count);

    if (*reader_count == 1){
        sem_wait(file_write);
        printf("%c LOCKED WRITE\n", name);
    }
    sem_post(file_read);
    
    printf("Reader %c  is in.\n", name);
    sleep(4);

    // last reader
    sem_wait(file_read);
    *reader_count = *reader_count - 1;
    printf("%c exiting: render_count: %d\n", name, *reader_count);
    if (*reader_count == 0){
        sem_post(file_write);
        printf("%c UNLOCKED WRITE\n",name);
    }
    sem_post(file_read);

    printf("Reader %c  successfully exited\n", name);
    munmap(reader_count, sizeof(int));
    close(shm_fd);

}