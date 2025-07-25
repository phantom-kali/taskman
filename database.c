#include "database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sqlite3 *db = NULL;
static const char *DB_FILENAME = "tasks.db";

// SQL statements
static const char *CREATE_TABLE_SQL = 
    "CREATE TABLE IF NOT EXISTS tasks ("
    "id INTEGER PRIMARY KEY,"
    "description TEXT NOT NULL,"
    "completed INTEGER NOT NULL DEFAULT 0,"
    "created INTEGER NOT NULL"
    ");";

static const char *INSERT_TASK_SQL = 
    "INSERT INTO tasks (id, description, completed, created) VALUES (?, ?, ?, ?);";

static const char *UPDATE_TASK_SQL = 
    "UPDATE tasks SET description = ?, completed = ? WHERE id = ?;";

static const char *DELETE_TASK_SQL = 
    "DELETE FROM tasks WHERE id = ?;";

static const char *SELECT_ALL_TASKS_SQL = 
    "SELECT id, description, completed, created FROM tasks ORDER BY created ASC;";

static const char *SELECT_MAX_ID_SQL = 
    "SELECT MAX(id) FROM tasks;";

int db_init(void)
{
    int rc = sqlite3_open(DB_FILENAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Create table if it doesn't exist
    char *err_msg = NULL;
    rc = sqlite3_exec(db, CREATE_TABLE_SQL, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot create table: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        db = NULL;
        return -1;
    }

    return 0;
}

void db_close(void)
{
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_load_tasks(TaskManager *tm)
{
    if (!db || !tm) {
        return -1;
    }

    tm->count = 0;
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, SELECT_ALL_TASKS_SQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot prepare select statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && tm->count < MAX_TASKS) {
        Task *task = &tm->tasks[tm->count];
        
        task->id = sqlite3_column_int(stmt, 0);
        const char *desc = (const char *)sqlite3_column_text(stmt, 1);
        strncpy(task->description, desc, MAX_TASK_LENGTH - 1);
        task->description[MAX_TASK_LENGTH - 1] = '\0';
        task->completed = (Status)sqlite3_column_int(stmt, 2);
        task->created = (time_t)sqlite3_column_int64(stmt, 3);
        
        tm->count++;
    }

    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        fprintf(stderr, "Error: Cannot load tasks: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int db_save_task(const Task *task)
{
    if (!db || !task) {
        return -1;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, INSERT_TASK_SQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot prepare insert statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, task->id);
    sqlite3_bind_text(stmt, 2, task->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, (int)task->completed);
    sqlite3_bind_int64(stmt, 4, (sqlite3_int64)task->created);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error: Cannot save task: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int db_update_task(const Task *task)
{
    if (!db || !task) {
        return -1;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, UPDATE_TASK_SQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot prepare update statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, task->description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, (int)task->completed);
    sqlite3_bind_int(stmt, 3, task->id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error: Cannot update task: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int db_delete_task(int id)
{
    if (!db) {
        return -1;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, DELETE_TASK_SQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot prepare delete statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error: Cannot delete task: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}

int db_get_next_id(void)
{
    if (!db) {
        return 1;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, SELECT_MAX_ID_SQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: Cannot prepare max id statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    int max_id = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
            max_id = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);
    return max_id + 1;
}
