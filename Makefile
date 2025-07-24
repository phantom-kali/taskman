CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = taskman
SOURCE = taskman.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) tasks.txt

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install
