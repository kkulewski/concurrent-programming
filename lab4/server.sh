#!/bin/bash

trap "" SIGHUP SIGTERM SIGCHLD
trap "exit 0" SIGUSR1

SERVER_FIFO="$HOME/server_fifo"

if [ -p $SERVER_FIFO ]; then
    rm $SERVER_FIFO
fi
mkfifo $SERVER_FIFO
chmod 777 $SERVER_FIFO

function handle_request {
    local CLIENT_FIFO=$1
    local REQUEST_ARG=$2
    echo $(($REQUEST_ARG*$REQUEST_ARG)) > $CLIENT_FIFO
}

while true; do
    read REQUEST < $SERVER_FIFO
    if [ -n "$REQUEST" ]; then
        REQUEST_ARR=($REQUEST)
        CLIENT_FIFO=${REQUEST_ARR[0]}
        NUMBER=${REQUEST_ARR[1]}
        handle_request $CLIENT_FIFO $NUMBER &
    fi
done
exit 0
