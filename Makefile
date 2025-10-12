CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lsqlite3
TARGET = taskman
SOURCES = taskman.c database.c search.c
OBJECTS = $(SOURCES:.c=.o)
PREFIX = $(HOME)/.local
BINDIR = $(PREFIX)/bin
COMPLETION_DIR = $(HOME)/.local/share/bash-completion/completions

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	install -d $(dir $(COMPLETION_DIR))
	install -m 644 taskman-completion.bash $(COMPLETION_DIR)/taskman
	@echo "Bash completion installed to $(COMPLETION_DIR)/taskman"
	@echo "TaskMan installed successfully!"
	@echo "Binary location: $(BINDIR)/taskman"
	@echo "Database will be stored in ~/.taskman/tasks.db"
	@echo "Run 'taskman help' to get started"
	@echo "Make sure $(BINDIR) is in your PATH"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(COMPLETION_DIR)/taskman
	@echo "TaskMan uninstalled"
	@echo "Note: Database in ~/.taskman/ was not removed"

.PHONY: clean install uninstall
