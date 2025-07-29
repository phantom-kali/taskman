#!/bin/bash
# Bash completion for taskman

_taskman_completion() {
    local cur prev words cword
    _init_completion || return

    local commands="add list list-all search done delete edit status help"

    case $prev in
        taskman)
            COMPREPLY=($(compgen -W "$commands" -- "$cur"))
            return 0
            ;;
        done|delete|edit)
            # For done, delete, and edit commands, we could potentially
            # complete with task IDs, but that would require calling taskman
            # which might be slow. For now, just return empty.
            return 0
            ;;
        *)
            return 0
            ;;
    esac
}

complete -F _taskman_completion taskman
