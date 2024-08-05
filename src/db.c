#include "../include/db.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool db_open(Database** db, const char* path) {
    sqlite3* sqlite_db;
    if (sqlite3_open(path, &sqlite_db) != SQLITE_OK) {
        return false;
    }

    *db = malloc(sizeof(Database));
    (*db)->db = sqlite_db;

    const char* schema_query =
        "CREATE TABLE IF NOT EXISTS kv ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "key TEXT NOT NULL UNIQUE,"
            "value TEXT NOT NULL"
        ");";

    if (sqlite3_exec((*db)->db, schema_query, NULL, NULL, NULL) != SQLITE_OK) {
        sqlite3_close((*db)->db);
        return false;
    }

    (*db)->path = strdup(path);

    return true;
}

void db_close(Database* db) {
    sqlite3_close(db->db);
    free(db->path);
}

bool db_set(Database* db, const char* key, const char* value) {
    const char* query =
        "INSERT OR REPLACE INTO kv (key, value) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->db, query, -1, &stmt, NULL) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

char* db_get(Database* db, const char* key) {
    const char* query =
        "SELECT value FROM kv WHERE key = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->db, query, -1, &stmt, NULL) != SQLITE_OK) {
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    const char* value = (const char*)sqlite3_column_text(stmt, 0);
    char* result = strdup(value);

    sqlite3_finalize(stmt);
    return result;
}

bool db_del(Database* db, const char* key) {
    const char* query =
        "DELETE FROM kv WHERE key = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->db, query, -1, &stmt, NULL) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}


