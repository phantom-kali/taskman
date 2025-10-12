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

# Install (user-space, no sudo required)
echo "Installing TaskMan to user space..."
make install

# Setup completion
echo "Setting up bash completion..."
COMPLETION_FILE="$HOME/.local/share/bash-completion/completions/taskman"
# Completion is already installed by make install, just ensure bashrc sources it
if ! grep -q "source.*bash_completion" "$HOME/.bashrc" 2>/dev/null; then
    echo "# Enable bash completion" >> "$HOME/.bashrc"
    echo "if [ -f /usr/share/bash-completion/bash_completion ]; then" >> "$HOME/.bashrc"
    echo "    . /usr/share/bash-completion/bash_completion" >> "$HOME/.bashrc"
    echo "elif [ -f /etc/bash_completion ]; then" >> "$HOME/.bashrc"
    echo "    . /etc/bash_completion" >> "$HOME/.bashrc"
    echo "fi" >> "$HOME/.bashrc"
    echo "Added bash completion support to ~/.bashrc"
fi

echo ""
echo "✅ TaskMan setup complete!"
echo ""
echo "What's been installed:"
echo "• TaskMan binary: $HOME/.local/bin/taskman"
echo "• Database location: ~/.taskman/tasks.db"
echo "• Bash completion: $HOME/.local/share/bash-completion/completions/taskman"
echo ""
echo "Next steps:"
echo "1. Add $HOME/.local/bin to your PATH if not already:"
echo "   export PATH=\"$HOME/.local/bin:\$PATH\""
echo "2. Restart your shell or run: source ~/.bashrc"
echo "3. Start using TaskMan: taskman add \"My first task\""
echo "4. Run 'taskman help' for more commands"
echo ""
echo "Your tasks will be stored in ~/.taskman/ and accessible from anywhere!"
