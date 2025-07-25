CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lsqlite3
TARGET = taskman
SOURCES = taskman.c database.c search.c
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) tasks.db tasks.txt

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install
