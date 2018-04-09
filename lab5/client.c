#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CLIENT_FIFO "client_fifo"

typedef struct request_t
{
    unsigned int length;
    int id;
    char* homepath;
} request;

void sendRequest(int fifoHandle, request* req)
{
    unsigned char* buffer;

    // alloc memory for whole request
    buffer = (char*) malloc(sizeof(req->length) + req->length);

    // copy request length to buffer
    memcpy(buffer, &req->length, sizeof(req->length));

    // copy request ID to buffer
    memcpy(buffer + sizeof(req->length), &req->id, sizeof(req->id));

    // copy request homepath to buffer (substract id from length because length == id + homepath)
    memcpy(buffer + sizeof(req->length) + sizeof(req->id), req->homepath, req->length - sizeof(req->id));

    write(fifoHandle, buffer, req->length + sizeof(req->id));
    free(buffer);
}

int main(int argc, char * argv[])
{
    int clientFifoHandle = open(CLIENT_FIFO, O_WRONLY);

    request req;
    req.id = atoi(argv[1]);
    req.homepath = getenv("HOME");
    req.length = sizeof(req.id) + strlen(req.homepath);

    sendRequest(clientFifoHandle, &req);
}
