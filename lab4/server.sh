#!/bin/bash

SERVER_ADDRESS=$1

CLIENT_FIFO="$HOME/client_fifo"
SERVER_FIFO="$HOME/server_fifo"

rm $SERVER_FIFO
mkfifo $SERVER_FIFO
chmod 777 $SERVER_FIFO

while true; do
  read CLIENT_ADDRESS < $SERVER_FIFO
  read ARGUMENT < $SERVER_FIFO
  echo $(($ARGUMENT*2)) > $CLIENT_FIFO
done
exit 0
