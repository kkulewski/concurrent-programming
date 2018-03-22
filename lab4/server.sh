#!/bin/bash

trap "" SIGHUP SIGTERM SIGCHLD
trap "exit 0" SIGUSR1

SERVER_FIFO="$HOME/server_fifo"

if [ -p $SERVER_FIFO ]; then
    rm $SERVER_FIFO
fi
mkfifo $SERVER_FIFO
chmod 777 $SERVER_FIFO


function handler {
    local CLIENT_FIFO=$1
    local REQUEST_ARG=$2

    echo "[HANDLER]: ARG == $REQUEST_ARG"
    echo "[HANDLER]: ADR == $CLIENT_FIFO"

    echo "[HANDLER]: sending response..."
    echo $(($REQUEST_ARG*$REQUEST_ARG)) > $CLIENT_FIFO
    echo "[HANDLER]: response sent!"
}

echo "[SERVER]: waiting for request..."
while true; do
    read REQUEST < $SERVER_FIFO
    if [ -n "$REQUEST" ]; then
        REQUEST_ARR=($REQUEST)
        CLIENT_FIFO=${REQUEST_ARR[0]}
        NUMBA=${REQUEST_ARR[1]}
        echo "[SERVER]: forked - [$REQUEST]"
        handler $CLIENT_FIFO $NUMBA &
    fi
    sleep 3                     # WHY
done
exit 0
