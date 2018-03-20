#!/bin/bash

SERVER_ADDRESS=$1
ARGUMENT=$2

CLIENT_FIFO="$HOME/client_fifo"
SERVER_FIFO="$HOME/server_fifo"

rm $CLIENT_FIFO
mkfifo $CLIENT_FIFO
chmod 777 $CLIENT_FIFO

echo $HOME > $SERVER_FIFO
echo $ARGUMENT > $SERVER_FIFO

while [[ $RESPONSE -le 0 ]]; do
  read RESPONSE < $CLIENT_FIFO
  echo $RESPONSE
done
exit 0
