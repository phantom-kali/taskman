#ifndef DATABASE_H
#define DATABASE_H

#include <time.h>

#define MAX_TASK_LENGTH 256
#define MAX_TASKS 100

typedef enum
{
    TODO = 0,
    DONE = 1
} Status;

typedef struct
{
    int id;
    char description[MAX_TASK_LENGTH];
    Status completed;
    time_t created;
} Task;

typedef struct
{
    Task tasks[MAX_TASKS];
    int count;
} TaskManager;

// Database operations
int db_init(void);
void db_close(void);
int db_load_tasks(TaskManager *tm);
int db_save_task(const Task *task);
int db_update_task(const Task *task);
int db_delete_task(int id);
int db_get_next_id(void);
int db_search_tasks(TaskManager *tm, const char *search_term);
const char *db_get_path(void);

#endif // DATABASE_H
