# TaskMan

A lightweight command-line task manager written in C for organizing daily tasks.

## Installation

```bash
git clone https://github.com/phantom-kali/taskman.git
cd taskman
make
```

## Usage

```bash
# Add a task
./taskman add "Complete project documentation"

# List pending tasks
./taskman list

# List all tasks (including completed)
./taskman list-all

# Mark task as completed
./taskman done <id>

# Edit a task description
./taskman edit <id> "New description"

# Delete a task
./taskman delete <id>

# Show help
./taskman help
```

## Example

```bash
$ ./taskman add "Buy groceries"
Task added: #1 - Buy groceries

$ ./taskman add "Finish assignment"
Task added: #2 - Finish assignment

$ ./taskman list
ID   Status   Created              Description
------------------------------------------------------------
1    [TODO]   2024-07-25 14:30     Buy groceries
2    [TODO]   2024-07-25 14:31     Finish assignment

$ ./taskman done 1
Task #1 marked as completed!

$ ./taskman edit 2 "Finish math assignment"
Task #2 updated.

$ ./taskman delete 1
Are you sure you want to delete task #1? (y/n): y
Task #1 deleted.
```

## Features

- Persistent task storage (with safe file writes using a temporary file)
- Task completion tracking
- Timestamps for task creation
- Colored terminal output for status indicators
- Edit task descriptions
- Sorted listing by creation time
- Confirmation prompt before task deletion
- Clean command-line interface
- No external dependencies

## Requirements

- GCC compiler
- Make utility

## Build

```bash
make clean  # Remove existing builds
make        # Compile
```

Tasks are stored in `tasks.txt` in the current directory.
