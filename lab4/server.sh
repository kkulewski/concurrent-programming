#!/bin/bash

trap "" SIGHUP SIGTERM SIGCHLD
trap "exit 0" SIGUSR1

SERVER_FIFO="$HOME/server_fifo"

if [ -p $SERVER_FIFO ]; then
    rm $SERVER_FIFO
fi
mkfifo $SERVER_FIFO
chmod 777 $SERVER_FIFO

echo "[SERVER]: waiting for request..."
while true; do
    read REQUEST < $SERVER_FIFO
    if [ -n "$REQUEST" ]; then
        echo "[SERVER]: forked - [$REQUEST]"
        ./handle_request.sh $REQUEST &
    fi
    sleep 3
done
exit 0
