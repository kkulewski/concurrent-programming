## Client - Server communication using IPC queues
Compile:

`gcc -o client client.c`

`gcc -o server server.c`


Run:

`./server`

`./client {WORD}`

### Related

List IPC queues:

`ipcs -q`

Remove IPC queue:

`ipcrm -Q {KEY}`
