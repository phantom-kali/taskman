#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TASK_LENGTH 256
#define MAX_TASKS 100
#define FILENAME "tasks.txt"

typedef struct {
    int id;
    char description[MAX_TASK_LENGTH];
    int completed;
    time_t created;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int count;
} TaskManager;

TaskManager tm = {0};

void load_tasks() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;
    
    while (tm.count < MAX_TASKS && 
           fscanf(file, "%d|%d|%ld|%[^\n]\n", 
                  &tm.tasks[tm.count].id,
                  &tm.tasks[tm.count].completed,
                  &tm.tasks[tm.count].created,
                  tm.tasks[tm.count].description) == 4) {
        tm.count++;
    }
    fclose(file);
}

void save_tasks() {
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        printf("Error: Cannot save tasks\n");
        return;
    }
    
    for (int i = 0; i < tm.count; i++) {
        fprintf(file, "%d|%d|%ld|%s\n",
                tm.tasks[i].id,
                tm.tasks[i].completed,
                tm.tasks[i].created,
                tm.tasks[i].description);
    }
    fclose(file);
}

int get_next_id() {
    int max_id = 0;
    for (int i = 0; i < tm.count; i++) {
        if (tm.tasks[i].id > max_id) {
            max_id = tm.tasks[i].id;
        }
    }
    return max_id + 1;
}

void add_task(const char *description) {
    if (tm.count >= MAX_TASKS) {
        printf("Error: Maximum tasks reached\n");
        return;
    }
    
    Task *task = &tm.tasks[tm.count];
    task->id = get_next_id();
    strncpy(task->description, description, MAX_TASK_LENGTH - 1);
    task->description[MAX_TASK_LENGTH - 1] = '\0';
    task->completed = 0;
    task->created = time(NULL);
    
    tm.count++;
    save_tasks();
    printf("Task added: #%d - %s\n", task->id, task->description);
}

void list_tasks(int show_completed) {
    if (tm.count == 0) {
        printf("No tasks found.\n");
        return;
    }
    
    printf("\n%-4s %-6s %-20s %s\n", "ID", "Status", "Created", "Description");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < tm.count; i++) {
        Task *task = &tm.tasks[i];
        
        if (!show_completed && task->completed) continue;
        
        char time_str[20];
        struct tm *tm_info = localtime(&task->created);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);
        
        printf("%-4d %-6s %-20s %s\n",
               task->id,
               task->completed ? "[DONE]" : "[TODO]",
               time_str,
               task->description);
    }
}

void complete_task(int id) {
    for (int i = 0; i < tm.count; i++) {
        if (tm.tasks[i].id == id) {
            tm.tasks[i].completed = 1;
            save_tasks();
            printf("Task #%d marked as completed!\n", id);
            return;
        }
    }
    printf("Task #%d not found.\n", id);
}

void delete_task(int id) {
    for (int i = 0; i < tm.count; i++) {
        if (tm.tasks[i].id == id) {
            // Shift remaining tasks
            for (int j = i; j < tm.count - 1; j++) {
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

void show_help() {
    printf("\nSimple Task Manager\n");
    printf("==================\n");
    printf("Usage:\n");
    printf("  ./taskman add \"Task description\"    - Add a new task\n");
    printf("  ./taskman list                      - List pending tasks\n");
    printf("  ./taskman list-all                  - List all tasks\n");
    printf("  ./taskman done <id>                 - Mark task as completed\n");
    printf("  ./taskman delete <id>               - Delete a task\n");
    printf("  ./taskman help                      - Show this help\n\n");
}

int main(int argc, char *argv[]) {
    load_tasks();
    
    if (argc < 2) {
        show_help();
        return 1;
    }
    
    if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Error: Please provide task description\n");
            printf("Usage: ./taskman add \"Task description\"\n");
            return 1;
        }
        add_task(argv[2]);
        
    } else if (strcmp(argv[1], "list") == 0) {
        list_tasks(0);
        
    } else if (strcmp(argv[1], "list-all") == 0) {
        list_tasks(1);
        
    } else if (strcmp(argv[1], "done") == 0) {
        if (argc < 3) {
            printf("Error: Please provide task ID\n");
            printf("Usage: ./taskman done <id>\n");
            return 1;
        }
        complete_task(atoi(argv[2]));
        
    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc < 3) {
            printf("Error: Please provide task ID\n");
            printf("Usage: ./taskman delete <id>\n");
            return 1;
        }
        delete_task(atoi(argv[2]));
        
    } else if (strcmp(argv[1], "help") == 0) {
        show_help();
        
    } else {
        printf("Unknown command: %s\n", argv[1]);
        show_help();
        return 1;
    }
    
    return 0;
}
