#ifndef SEARCH_H
#define SEARCH_H

#include "database.h"

// Terminal control sequences
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR_HOME "\033[H"
#define CLEAR_LINE "\033[K"
#define SAVE_CURSOR "\033[s"
#define RESTORE_CURSOR "\033[u"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

// Key codes
#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_BACKSPACE 127
#define KEY_CTRL_C 3

// Search function
void interactive_search(void);
void display_search_results(const TaskManager *tm, const char *search_term, int highlight_index);
int getch(void);
void enable_raw_mode(void);
void disable_raw_mode(void);

#endif // SEARCH_H
