#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

static struct termios orig_termios;

// Case-insensitive string comparison (portable implementation)
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n-- > 0) {
        int c1 = tolower((unsigned char)*s1++);
        int c2 = tolower((unsigned char)*s2++);
        if (c1 != c2) return c1 - c2;
        if (c1 == '\0') return 0;
    }
    return 0;
}

// Case-insensitive string search (portable implementation)
char *strcasestr(const char *haystack, const char *needle)
{
    if (!haystack || !needle) return NULL;
    if (*needle == '\0') return (char *)haystack;
    
    size_t needle_len = strlen(needle);
    for (const char *p = haystack; *p; p++) {
        if (strncasecmp(p, needle, needle_len) == 0) {
            return (char *)p;
        }
    }
    return NULL;
}

void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int getch(void)
{
    int ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        return ch;
    }
    return -1;
}

void display_search_results(const TaskManager *tm, const char *search_term, int highlight_index)
{
    printf(CLEAR_SCREEN MOVE_CURSOR_HOME);
    printf("TaskMan Interactive Search\n");
    printf("==========================\n");
    printf("Search: %s%s\n", search_term, strlen(search_term) > 0 ? "_" : "_");
    printf("Press ESC to exit, Enter to select, Ctrl+C to cancel\n\n");
    
    if (tm->count == 0) {
        if (strlen(search_term) == 0) {
            printf("Start typing to search for tasks...\n");
        } else {
            printf("No tasks found matching '%s'\n", search_term);
        }
        return;
    }
    
    printf("%-4s %-8s %-20s %s\n", "ID", "Status", "Created", "Description");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < tm->count; i++) {
        const Task *task = &tm->tasks[i];
        char time_str[20];
        struct tm *tm_info = localtime(&task->created);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);
        
        // Highlight the selected task
        if (i == highlight_index) {
            printf("\033[7m"); // Reverse video
        }
        
        printf("%-4d \033[0;%sm%-8s\033[0m %-20s %s",
               task->id,
               task->completed == DONE ? "32" : "33",
               task->completed == DONE ? "[DONE]" : "[TODO]",
               time_str,
               task->description);
        
        if (i == highlight_index) {
            printf("\033[0m"); // Reset formatting
        }
        printf("\n");
    }
    
    printf("\nFound %d task(s). Use ↑/↓ arrows to navigate, Enter to select.\n", tm->count);
}

void highlight_search_term(const char *text, const char *search_term)
{
    if (strlen(search_term) == 0) {
        printf("%s", text);
        return;
    }
    
    char *found = strcasestr(text, search_term);
    if (!found) {
        printf("%s", text);
        return;
    }
    
    // Print text before match
    printf("%.*s", (int)(found - text), text);
    // Print highlighted match
    printf("\033[1;33m%.*s\033[0m", (int)strlen(search_term), found);
    // Print text after match (recursively highlight remaining matches)
    highlight_search_term(found + strlen(search_term), search_term);
}

void interactive_search(void)
{
    char search_term[MAX_TASK_LENGTH] = {0};
    int search_len = 0;
    int highlight_index = 0;
    TaskManager search_results = {0};
    
    printf(HIDE_CURSOR);
    enable_raw_mode();
    
    // Initial display
    if (db_load_tasks(&search_results) == 0) {
        display_search_results(&search_results, search_term, highlight_index);
    }
    
    while (1) {
        int ch = getch();
        
        switch (ch) {
            case KEY_ESC:
                // Check if this is an arrow key sequence
                {
                    int next_ch = getch();
                    if (next_ch == 91) { // [
                        int arrow = getch();
                        switch (arrow) {
                            case 65: // Up arrow
                                if (highlight_index > 0) {
                                    highlight_index--;
                                    display_search_results(&search_results, search_term, highlight_index);
                                }
                                break;
                            case 66: // Down arrow
                                if (highlight_index < search_results.count - 1) {
                                    highlight_index++;
                                    display_search_results(&search_results, search_term, highlight_index);
                                }
                                break;
                        }
                    } else {
                        // True ESC key, exit
                        goto cleanup;
                    }
                }
                break;
                
            case KEY_CTRL_C:
                printf(SHOW_CURSOR);
                disable_raw_mode();
                printf("\nSearch cancelled.\n");
                return;
                
            case KEY_ENTER:
                if (search_results.count > 0 && highlight_index < search_results.count) {
                    printf(SHOW_CURSOR);
                    disable_raw_mode();
                    printf(CLEAR_SCREEN MOVE_CURSOR_HOME);
                    
                    // Display selected task details
                    Task *selected = &search_results.tasks[highlight_index];
                    char time_str[20];
                    struct tm *tm_info = localtime(&selected->created);
                    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);
                    
                    printf("Selected Task:\n");
                    printf("==============\n");
                    printf("ID: %d\n", selected->id);
                    printf("Status: %s\n", selected->completed == DONE ? "DONE" : "TODO");
                    printf("Created: %s\n", time_str);
                    printf("Description: %s\n\n", selected->description);
                    
                    printf("What would you like to do?\n");
                    printf("1. Mark as %s\n", selected->completed == DONE ? "TODO" : "DONE");
                    printf("2. Edit description\n");
                    printf("3. Delete task\n");
                    printf("4. Return to search\n");
                    printf("5. Exit\n");
                    printf("Choice (1-5): ");
                    
                    char choice = getchar();
                    getchar(); // consume newline
                    
                    switch (choice) {
                        case '1':
                            selected->completed = (selected->completed == DONE) ? TODO : DONE;
                            if (db_update_task(selected) == 0) {
                                printf("Task status updated!\n");
                            }
                            break;
                        case '2':
                            printf("Enter new description: ");
                            char new_desc[MAX_TASK_LENGTH];
                            if (fgets(new_desc, sizeof(new_desc), stdin)) {
                                // Remove newline
                                new_desc[strcspn(new_desc, "\n")] = 0;
                                strncpy(selected->description, new_desc, MAX_TASK_LENGTH - 1);
                                selected->description[MAX_TASK_LENGTH - 1] = '\0';
                                if (db_update_task(selected) == 0) {
                                    printf("Task description updated!\n");
                                }
                            }
                            break;
                        case '3':
                            printf("Are you sure you want to delete this task? (y/n): ");
                            char confirm = getchar();
                            getchar(); // consume newline
                            if (confirm == 'y' || confirm == 'Y') {
                                if (db_delete_task(selected->id) == 0) {
                                    printf("Task deleted!\n");
                                }
                            }
                            break;
                        case '4':
                            interactive_search();
                            return;
                        case '5':
                            return;
                    }
                    return;
                }
                break;
                
            case KEY_BACKSPACE:
                if (search_len > 0) {
                    search_len--;
                    search_term[search_len] = '\0';
                    highlight_index = 0;
                    
                    if (strlen(search_term) == 0) {
                        db_load_tasks(&search_results);
                    } else {
                        db_search_tasks(&search_results, search_term);
                    }
                    display_search_results(&search_results, search_term, highlight_index);
                }
                break;
                
            default:
                if (ch >= 32 && ch <= 126 && search_len < MAX_TASK_LENGTH - 1) {
                    search_term[search_len] = ch;
                    search_len++;
                    search_term[search_len] = '\0';
                    highlight_index = 0;
                    
                    db_search_tasks(&search_results, search_term);
                    display_search_results(&search_results, search_term, highlight_index);
                }
                break;
        }
    }
    
cleanup:
    printf(SHOW_CURSOR);
    disable_raw_mode();
    printf(CLEAR_SCREEN MOVE_CURSOR_HOME);
    printf("Search exited.\n");
}
