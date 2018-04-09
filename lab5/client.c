#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CLIENT_FIFO "client_fifo"

typedef struct request_t
{
    int payloadSize;

    // payload
    int id;
    char* homepath;

} request;

void sendRequest(int fifoHandle, request* req)
{
    int requestSize = sizeof(req->payloadSize) + req->payloadSize;
    void* buffer;
    buffer = malloc(requestSize);

    memcpy(buffer, &req->payloadSize, sizeof(req->payloadSize));
    memcpy(buffer + sizeof(req->payloadSize), &req->id, sizeof(req->id));
    memcpy(buffer + sizeof(req->payloadSize) + sizeof(req->id), req->homepath, req->payloadSize - sizeof(req->id));

    write(fifoHandle, buffer, requestSize);
    free(buffer);
}

int main(int argc, char * argv[])
{
    int clientFifoHandle = open(CLIENT_FIFO, O_WRONLY);

    request req;
    req.id = atoi(argv[1]);
    req.homepath = getenv("HOME");
    req.payloadSize = sizeof(req.id) + strlen(req.homepath);

    sendRequest(clientFifoHandle, &req);
}
