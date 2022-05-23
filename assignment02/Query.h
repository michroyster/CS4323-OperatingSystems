#ifndef QUERY_H
#define QUERY_H

// This struct will be used to pass the query information from the Manager to the Assistant

typedef struct Query {
    char employee_name[50];
    char job_title[64];
    char status[3];
} Query;

#endif