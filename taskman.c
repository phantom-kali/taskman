#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TASK_LENGTH 256
#define MAX_TASKS 100
#define FILENAME "tasks.txt"
#define TEMPFILE "tasks.tmp"

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

TaskManager tm = {0};

void load_tasks()
{
    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        perror("Failed to open task file");
        return;
    }

    while (tm.count < MAX_TASKS &&
           fscanf(file, "%d|%d|%ld|%[^]\n",
                  &tm.tasks[tm.count].id,
                  (int *)&tm.tasks[tm.count].completed,
                  &tm.tasks[tm.count].created,
                  tm.tasks[tm.count].description) == 4)
    {
        tm.count++;
    }
    fclose(file);
}

void save_tasks()
{
    FILE *temp = fopen(TEMPFILE, "w");
    if (!temp)
    {
        printf("Error: Cannot save tasks\n");
        return;
    }

    for (int i = 0; i < tm.count; i++)
    {
        fprintf(temp, "%d|%d|%ld|%s\n",
                tm.tasks[i].id,
                tm.tasks[i].completed,
                tm.tasks[i].created,
                tm.tasks[i].description);
    }
    fclose(temp);
    remove(FILENAME);
    rename(TEMPFILE, FILENAME);
}

int get_next_id()
{
    int max_id = 0;
    for (int i = 0; i < tm.count; i++)
    {
        if (tm.tasks[i].id > max_id)
        {
            max_id = tm.tasks[i].id;
        }
    }
    return max_id + 1;
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

    Task *task = &tm.tasks[tm.count];
    task->id = get_next_id();
    strncpy(task->description, description, MAX_TASK_LENGTH - 1);
    task->description[MAX_TASK_LENGTH - 1] = '\0';
    task->completed = TODO;
    task->created = time(NULL);

    tm.count++;
    save_tasks();
    printf("Task added: #%d - %s\n", task->id, task->description);
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
            save_tasks();
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
            for (int j = i; j < tm.count - 1; j++)
            {
                tm.tasks[j] = tm.tasks[j + 1];
            }
            tm.count--;
            save_tasks();
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
            save_tasks();
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
    printf("  ./taskman add \"Task description\"    - Add a new task\n");
    printf("  ./taskman list                         - List pending tasks\n");
    printf("  ./taskman list-all                     - List all tasks\n");
    printf("  ./taskman done <id>                    - Mark task as completed\n");
    printf("  ./taskman delete <id>                  - Delete a task\n");
    printf("  ./taskman edit <id> \"new description\" - Edit a task\n");
    printf("  ./taskman help                         - Show this help\n\n");
}

int main(int argc, char *argv[])
{
    load_tasks();

    if (argc < 2)
    {
        show_help();
        return 1;
    }

    if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task description\n");
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
    else if (strcmp(argv[1], "done") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task ID\n");
            return 1;
        }
        complete_task(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "delete") == 0)
    {
        if (argc < 3)
        {
            printf("Error: Please provide task ID\n");
            return 1;
        }
        delete_task(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "edit") == 0)
    {
        if (argc < 4)
        {
            printf("Usage: ./taskman edit <id> \"new description\"\n");
            return 1;
        }
        edit_task(atoi(argv[2]), argv[3]);
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        show_help();
    }
    else
    {
        printf("Unknown command: %s\n", argv[1]);
        show_help();
        return 1;
    }

    return 0;
}
