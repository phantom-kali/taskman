#!/bin/bash
# Setup script for TaskMan

set -e

echo "Setting up TaskMan..."

# Check dependencies
if ! command -v gcc &> /dev/null; then
    echo "Error: GCC compiler not found. Please install build-essential or development tools."
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: Make utility not found. Please install make."
    exit 1
fi

# Check for SQLite3 development libraries
if ! pkg-config --exists sqlite3 2>/dev/null; then
    echo "Error: SQLite3 development libraries not found."
    echo "Please install them:"
    echo "  Ubuntu/Debian: sudo apt-get install libsqlite3-dev"
    echo "  CentOS/RHEL:   sudo yum install sqlite-devel"
    echo "  Fedora:        sudo dnf install sqlite-devel"
    echo "  macOS:         brew install sqlite3"
    exit 1
fi

# Build the project
echo "Building TaskMan..."
make clean
make

# Install
echo "Installing TaskMan..."
sudo make install

# Setup completion
echo "Setting up bash completion..."
COMPLETION_FILE="$HOME/.taskman-completion"
cp taskman-completion.bash "$COMPLETION_FILE"

# Add to user's bashrc if not already there
if ! grep -q "source.*taskman-completion" "$HOME/.bashrc" 2>/dev/null; then
    echo "# TaskMan bash completion" >> "$HOME/.bashrc"
    echo "source $COMPLETION_FILE" >> "$HOME/.bashrc"
    echo "Added bash completion to ~/.bashrc"
fi

echo ""
echo "✅ TaskMan setup complete!"
echo ""
echo "What's been installed:"
echo "• TaskMan binary: /usr/local/bin/taskman"
echo "• Database location: ~/.taskman/tasks.db"
echo "• Bash completion: $COMPLETION_FILE"
echo ""
echo "Next steps:"
echo "1. Restart your shell or run: source ~/.bashrc"
echo "2. Start using TaskMan: taskman add \"My first task\""
echo "3. Run 'taskman help' for more commands"
echo ""
echo "Your tasks will be stored in ~/.taskman/ and accessible from anywhere!"
