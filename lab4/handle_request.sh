#!/bin/bash
CLIENT_FIFO=$1
REQUEST_ARG=$2

echo "[HANDLER]: ARG == $REQUEST_ARG"
echo "[HANDLER]: ADR == $CLIENT_FIFO"

echo "[HANDLER]: sending response..."
echo $(($REQUEST_ARG*$REQUEST_ARG)) > $CLIENT_FIFO
echo "[HANDLER]: response sent!"
exit 0
