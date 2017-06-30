#!/bin/bash
set -u

# Parameters:
# $1: Text to log
function log {
    echo "$(date) ${1:-}"
}

# Parameters:
# $1: process to be killed (if it is running)
function kill_process {
    if [ -z "${1:-}" ]; then
        echo "No process to kill specified"
    else
        ps aux | grep $1 | grep -v grep
        PID=`ps aux | grep "$1" | grep -v grep | awk '{print $2}'`
        if [ -n "$PID" ]; then
            echo "Killing process $PID"
            kill -9 $PID
        else
            echo "Nothing to kill"
        fi
    fi
}

# Parameters:
# $1: process to be checked
# Returns: PID if process is running, 0 otherwise
function is_process_running {
    if [ -z "${1:-}" ]; then
        echo "No process specified"
        return 0
    else
        PID=`ps aux | grep "$1" | grep -v grep | awk '{print $2}'`
        if [ -n "$PID" ]; then
            return 1
        else
            return 0
        fi
    fi
}

# Parameters:
# $1: file to be checked
# $2: new name if the file exists ($1.back if ommited)
# Returns: 0 if the file exists, 1 otherwise 
function rename_existing_file {
    if [ -e "${1:-}" ]; then
        mv $1 ${2:-$1.back}
        return 0
    else
        return 1
    fi 
}