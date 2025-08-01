CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lsqlite3
TARGET = taskman
SOURCES = taskman.c database.c search.c
OBJECTS = $(SOURCES:.c=.o)
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
COMPLETION_DIR = /etc/bash_completion.d

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	@if [ -d $(COMPLETION_DIR) ]; then \
		install -m 644 taskman-completion.bash $(COMPLETION_DIR)/taskman; \
		echo "Bash completion installed to $(COMPLETION_DIR)/taskman"; \
	else \
		echo "Bash completion directory not found. You can manually source taskman-completion.bash"; \
	fi
	@echo "TaskMan installed successfully!"
	@echo "Database will be stored in ~/.taskman/tasks.db"
	@echo "Run 'taskman help' to get started"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(COMPLETION_DIR)/taskman
	@echo "TaskMan uninstalled"

.PHONY: clean install uninstall
