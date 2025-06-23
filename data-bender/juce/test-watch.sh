#!/bin/bash

echo "Testing fswatch file watching..."
echo "Watching Source/ and ../core/ directories..."

# Start fswatch in background
fswatch -r Source/ ../core/ > /tmp/fswatch_output &
FSWATCH_PID=$!

echo "fswatch started with PID: $FSWATCH_PID"
echo "Make a change to any file in Source/ or ../core/ to test..."

# Monitor the output file
while true; do
    if [ -s /tmp/fswatch_output ]; then
        CHANGED_FILE=$(head -1 /tmp/fswatch_output)
        echo "Change detected: $CHANGED_FILE"
        > /tmp/fswatch_output
    fi
    sleep 0.1
done 