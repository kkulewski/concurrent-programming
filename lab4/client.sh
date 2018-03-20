#!/bin/bash
SERVER_ADDRESS=$1
ARGUMENT=$2
CLIENT_FIFO="$HOME/client_fifo"
SERVER_FIFO="/home/$SERVER_ADDRESS/server_fifo"

if [ -p $CLIENT_FIFO ]; then
    rm $CLIENT_FIFO
fi
mkfifo $CLIENT_FIFO
chmod 777 $CLIENT_FIFO

echo "[CLIENT]: sending request..."
echo $CLIENT_FIFO $ARGUMENT > $SERVER_FIFO

echo "[CLIENT]: waiting for response.."
while [ -z $RESPONSE ]; do
  read RESPONSE < $CLIENT_FIFO
done
echo $RESPONSE
rm $CLIENT_FIFO
exit 0
