#!/bin/bash

# Check and install necessary X11 and xvfb packages (requires root privileges if needed)
PACKAGES="xvfb libx11-6 libxi6 libxss1 libxtst6 libxrandr2 libasound2 libpangocairo-1.0-0 libatk1.0-0 libcairo-gobject2 libgtk-3-0 libgdk-pixbuf-xlib-2.0-0"

for pkg in $PACKAGES; do
    if ! dpkg -l $pkg &> /dev/null; then
        echo "Installing $pkg..."
        if [ "$EUID" -eq 0 ]; then
            apt-get update && apt-get install -y $pkg
        else
            echo "Root privileges required to install $pkg, trying sudo..."
            sudo apt-get update && sudo apt-get install -y --allow-downgrades $pkg
        fi
        break
    fi
done

# Start Xvfb virtual display
echo "Starting Xvfb virtual display..."
Xvfb :99 -screen 0 1024x768x24 -ac +extension GLX +render -noreset > /dev/null 2>&1 &
XVFB_PID=$!

# Wait for Xvfb to start
sleep 3

# Export DISPLAY environment variable
export DISPLAY=:99

# Start Celery worker
echo "Starting Celery worker..."
exec celery -A celery_config worker -Q simulation --loglevel=${CELERY_LOG_LEVEL:-info} --concurrency=${CELERY_CONCURRENCY:-2} --logfile=/home/jry/FSP/logs/celery.worker.log
