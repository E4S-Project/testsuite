#!/bin/bash

# 1. Ensure a target directory was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <target_directory>"
    exit 1
fi

TARGET_DIR="$1"

# 2. Verify the provided path is a valid directory
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: '$TARGET_DIR' is not a valid directory."
    exit 1
fi

# 3. Generate the timestamp (Format: YYYYMMDD_HHMMSS)
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_DIR_NAME="logs_$TIMESTAMP"

echo "Starting log cleanup in subdirectories of: $TARGET_DIR"
echo "Timestamp folder name: $LOG_DIR_NAME"
echo "------------------------------------------------"

# 4. Iterate through every immediate subdirectory
for dir in "$TARGET_DIR"/*/; do
    # Ensure it's a directory (safeguard if globbing finds nothing)
    [ -d "$dir" ] || continue

    # 5. Safe check for .log files using nullglob
    # This prevents the script from creating empty log folders if a test didn't generate logs.
    (
        shopt -s nullglob
        log_files=("$dir"/*.log)
        
        if [ ${#log_files[@]} -gt 0 ]; then
            NEW_LOG_PATH="$dir$LOG_DIR_NAME"
            
            # Create the logs_TIMESTAMP directory
            mkdir -p "$NEW_LOG_PATH"
            
            # Move all .log files into it
            mv "$dir"/*.log "$NEW_LOG_PATH/"
            
            echo "Organized ${#log_files[@]} log(s) in: $dir"
        fi
    )
done

echo "------------------------------------------------"
echo "All done."
