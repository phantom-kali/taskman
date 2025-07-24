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
ID   Status Created              Description
------------------------------------------------------------
1    [TODO] 2024-07-25 14:30     Buy groceries
2    [TODO] 2024-07-25 14:31     Finish assignment

$ ./taskman done 1
Task #1 marked as completed!
```

## Features

- Persistent task storage
- Task completion tracking
- Timestamps for task creation
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
