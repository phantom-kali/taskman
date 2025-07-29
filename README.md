# toTaskMan

A lightweight command-line task manager written in C with SQLite storage for organizing daily tasks.

## Requirements

- GCC compiler
- Make utility
- SQLite3 development libraries

### Installing SQLite3 development libraries

**Ubuntu/Debian:**

```bash
sudo apt-get install libsqlite3-dev
```

**CentOS/RHEL/Fedora:**

```bash
sudo yum install sqlite-devel  # CentOS/RHEL
sudo dnf install sqlite-devel  # Fedora
```

**macOS:**

```bash
brew install sqlite3
```

## Installation

### Quick Setup (Recommended)

```bash
git clone https://github.com/phantom-kali/taskman.git
cd taskman
./setup.sh
```

The setup script will:

- Check for required dependencies
- Build and install TaskMan system-wide
- Set up bash completion
- Configure the database location

### Manual Installation

```bash
git clone https://github.com/phantom-kali/taskman.git
cd taskman
make
sudo make install
```

### System Requirements

## Usage

```bash
# Add a task
taskman add "Complete project documentation"

# List pending tasks
taskman list

# List all tasks (including completed)
taskman list-all

# Interactive search (like Linux reverse search)
taskman search

# Mark task as completed
taskman done <id>

# Edit a task description
taskman edit <id> "New description"

# Delete a task
taskman delete <id>

# Show database location and statistics
taskman status

# Show help
taskman help
```

## Database Location

TaskMan stores all your tasks in `~/.taskman/tasks.db`. This means:

- Your tasks are accessible from anywhere on your system
- All tasks are stored in one central location
- Your tasks persist across different working directories

## Example

```bash
$ taskman add "Buy groceries"
Task added: #1 - Buy groceries

$ taskman add "Finish assignment"
Task added: #2 - Finish assignment

$ taskman list
ID   Status   Created              Description
------------------------------------------------------------
1    [TODO]   2024-07-25 14:30     Buy groceries
2    [TODO]   2024-07-25 14:31     Finish assignment

$ taskman done 1
Task #1 marked as completed!

$ taskman edit 2 "Finish math assignment"
Task #2 updated.

$ taskman status
TaskMan Status
==============
Database location: /home/user/.taskman/tasks.db
Total tasks: 2
Completed tasks: 1
Pending tasks: 1

$ taskman search
# Opens interactive search interface
# Type to search, use ↑/↓ arrows to navigate
# Press Enter to select a task for actions
# Press ESC to exit search
```

## Interactive Search Features

The `./taskman search` command provides a powerful interactive search experience:

- **Real-time filtering**: Start typing to instantly filter tasks
- **Navigation**: Use ↑/↓ arrow keys to navigate through search results
- **Task actions**: Press Enter on a selected task to:
  - Toggle completion status (TODO ↔ DONE)
  - Edit task description
  - Delete the task
  - Return to search
- **Exit options**: Press ESC to exit, Ctrl+C to cancel
- **Case-insensitive**: Search works regardless of letter case
- **Partial matching**: Finds tasks containing your search terms anywhere in the description

## Features

- **SQLite Database Storage**: Reliable, ACID-compliant data storage with better concurrent access
- **Interactive Search**: Real-time task filtering with Linux-style reverse search interface
- Task completion tracking
- Timestamps for task creation
- Colored terminal output for status indicators
- Edit task descriptions
- Sorted listing by creation time
- Confirmation prompt before task deletion
- Clean command-line interface
- Modular database layer for easy maintenance
- Prepared statements for security against SQL injection
- **Enhanced Navigation**: Arrow key navigation in search mode
- **Quick Actions**: Perform task operations directly from search results
- No external dependencies except SQLite

## Requirements

- GCC compiler
- Make utility
- SQLite3 development libraries

## Build

```bash
make clean  # Remove existing builds
make        # Compile
```

Tasks are stored in `~/.taskman/tasks.db` SQLite database, accessible from anywhere on your system.
