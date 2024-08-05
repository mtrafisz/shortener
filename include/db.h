#ifndef _DB_H
#define _DB_H

#include <stdbool.h>
#include <sqlite3.h>

typedef struct {
    sqlite3* db;
    char* path;
} Database;

bool db_open(Database** db, const char* path);
void db_close(Database* db);

bool db_set(Database* db, const char* key, const char* value);
char* db_get(Database* db, const char* key);
bool db_del(Database* db, const char* key);

#endif
