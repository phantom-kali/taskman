#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "database.h"
#include "search.h"

TaskManager tm = {0};

void load_tasks()
{
    if (db_load_tasks(&tm) != 0) {
        fprintf(stderr, "Warning: Could not load tasks from database\n");
    }
}

void save_task(Task *task)
{
    if (db_save_task(task) != 0) {
        fprintf(stderr, "Error: Cannot save task to database\n");
    }
}

void update_task(Task *task)
{
    if (db_update_task(task) != 0) {
        fprintf(stderr, "Error: Cannot update task in database\n");
    }
}

int get_next_id()
{
    return db_get_next_id();
}

void add_task(const char *description)
{
    if (tm.count >= MAX_TASKS)
    {
        printf("Error: Maximum tasks reached\n");
        return;
    }

    if (strlen(description) == 0 || strspn(description, " \t\n") == strlen(description))
    {
        printf("Error: Task description cannot be empty.\n");
        return;
    }

    Task task;
    task.id = get_next_id();
    strncpy(task.description, description, MAX_TASK_LENGTH - 1);
    task.description[MAX_TASK_LENGTH - 1] = '\0';
    task.completed = TODO;
    task.created = time(NULL);

    save_task(&task);
    
    // Add to local cache
    tm.tasks[tm.count] = task;
    tm.count++;
    
    printf("Task added: #%d - %s\n", task.id, task.description);
}

int cmp_created(const void *a, const void *b)
{
    return ((Task *)a)->created - ((Task *)b)->created;
}

void list_tasks(int show_completed)
{
    if (tm.count == 0)
    {
        printf("No tasks found.\n");
        return;
    }

    qsort(tm.tasks, tm.count, sizeof(Task), cmp_created);
    printf("\n%-4s %-8s %-20s %s\n", "ID", "Status", "Created", "Description");
    printf("------------------------------------------------------------\n");
    int displayed = 0;

    for (int i = 0; i < tm.count; i++)
    {
        Task *task = &tm.tasks[i];
        if (!show_completed && task->completed == DONE)
            continue;

        char time_str[20];
        struct tm *tm_info = localtime(&task->created);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);

        printf("%-4d \033[0;%sm%-8s\033[0m %-20s %s\n",
               task->id,
               task->completed == DONE ? "32" : "33",
               task->completed == DONE ? "[DONE]" : "[TODO]",
               time_str,
               task->description);
        displayed++;
    }
    printf("\nTotal tasks displayed: %d\n", displayed);
}

void complete_task(int id)
{
    for (int i = 0; i < tm.count; i++)
    {
        if (tm.tasks[i].id == id)
        {
            tm.tasks[i].completed = DONE;
            update_task(&tm.tasks[i]);
            printf("Task #%d marked as completed!\n", id);
            return;
        }
    }
    printf("Task #%d not found.\n", id);
}

void delete_task(int id)
{
    for (int i = 0; i < tm.count; i++)
    {
        if (tm.tasks[i].id == id)
        {
            printf("Are you sure you want to delete task #%d? (y/n): ", id);
            char confirm = getchar();
            getchar(); // consume newline
            if (confirm != 'y' && confirm != 'Y')
            {
                printf("Cancelled.\n");
                return;
            }
            
            // Delete from database
            if (db_delete_task(id) != 0) {
                printf("Error: Could not delete task from database.\n");
                return;
            }
            
            // Remove from local cache
            for (int j = i; j < tm.count - 1; j++)
            {
                tm.tasks[j] = tm.tasks[j + 1];
            }
            tm.count--;
            printf("Task #%d deleted.\n", id);
            return;
        }
    }
    printf("Task #%d not found.\n", id);
}

void edit_task(int id, const char *new_description)
{
    for (int i = 0; i < tm.count; i++)
    {
        if (tm.tasks[i].id == id)
        {
            strncpy(tm.tasks[i].description, new_description, MAX_TASK_LENGTH - 1);
            tm.tasks[i].description[MAX_TASK_LENGTH - 1] = '\0';
            update_task(&tm.tasks[i]);
            printf("Task #%d updated.\n", id);
            return;
        }
    }
    printf("Task #%d not found.\n", id);
}

void show_help()
{
    printf("\nSimple Task Manager\n");
    printf("==================\n");
    printf("Usage:\n");
    printf("  taskman add \"Task description\"    - Add a new task\n");
    printf("  taskman list                       - List pending tasks\n");
    printf("  taskman list-all                   - List all tasks\n");
    printf("  taskman search                     - Interactive search\n");
    printf("  taskman done <id>                  - Mark task as completed\n");
    printf("  taskman delete <id>                - Delete a task\n");
    printf("  taskman edit <id> \"new description\" - Edit a task\n");
    printf("  taskman status                     - Show database location and stats\n");
    printf("  taskman help                       - Show this help\n\n");
}

void show_status()
{
    printf("\nTaskMan Status\n");
    printf("==============\n");
    printf("Database location: %s\n", db_get_path());
    printf("Total tasks: %d\n", tm.count);
    
    int completed = 0, pending = 0;
    for (int i = 0; i < tm.count; i++) {
        if (tm.tasks[i].completed == DONE) {
            completed++;
        } else {
            pending++;
        }
    }
    
    printf("Completed tasks: %d\n", completed);
    printf("Pending tasks: %d\n", pending);
    printf("\n");
}

int main(int argc, char *argv[])
{
    // Initialize database
    if (db_init() != 0) {
        fprintf(stderr, "Error: Failed to initialize database\n");
        return 1;
    }

    load_tasks();

    if (argc < 2)
    {
        show_help();
        db_close();
        return 1;
    }

    if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task description\n");
            db_close();
            return 1;
        }
        add_task(argv[2]);
    }
    else if (strcmp(argv[1], "list") == 0)
    {
        list_tasks(0);
    }
    else if (strcmp(argv[1], "list-all") == 0)
    {
        list_tasks(1);
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        interactive_search();
    }
    else if (strcmp(argv[1], "done") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task ID\n");
            db_close();
            return 1;
        }
        complete_task(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "delete") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task ID\n");
            db_close();
            return 1;
        }
        delete_task(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "edit") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: ./taskman edit <id> \"new description\"\n");
            db_close();
            return 1;
        }
        edit_task(atoi(argv[2]), argv[3]);
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        show_help();
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        show_status();
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
        show_help();
        db_close();
        return 1;
    }

    db_close();
    return 0;
}
