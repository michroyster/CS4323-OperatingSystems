#ifndef RECORD_H
#define RECORD_H

// This struct will be used to pass query results from the Server

typedef struct Record {
    int id;
    char employee_name[50];
    char job_title[64];
    float base_pay;
    float overtime_pay;
    float benefit;
    char status[2];
    float satisfaction_level;
    int number_project;
    int average_monthly_hours;
    int time_spend_company_in_yrs;
    int work_accident;
    int promotion_last_5years;
} Record;

#endif