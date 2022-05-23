#ifndef HISTORY_H
#define HISTORY_H

#include "Query.h"
#include "Record.h"

void getFromHistory(Query search, Record *result);
bool isInHistory(Query search);
void writeToHistory(Record *record);

#endif