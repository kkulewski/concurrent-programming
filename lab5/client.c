#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CLIENT_FIFO "client_fifo"
#define SERVER_FIFO "server_fifo"

typedef struct request_t
{
    int payloadSize;

    // payload
    int id;
    char* homepath;

} request;

typedef struct response_t
{
    // payload
    char* name;

} response;

void sendRequest(int serverFifoHandle, request* req)
{
    int requestSize = sizeof(req->payloadSize) + req->payloadSize;
    void* buffer;
    buffer = malloc(requestSize);

    memcpy(buffer, &req->payloadSize, sizeof(req->payloadSize));
    memcpy(buffer + sizeof(req->payloadSize), &req->id, sizeof(req->id));
    memcpy(buffer + sizeof(req->payloadSize) + sizeof(req->id), req->homepath, req->payloadSize - sizeof(req->id));

    write(serverFifoHandle, buffer, requestSize);
    free(buffer);
}

response* receiveResponse(int clientFifoHandle, int responseSize)
{
    void* buffer = malloc(responseSize);
    response* res = malloc(responseSize);
    res->name = malloc(responseSize);

    read(clientFifoHandle, buffer, responseSize);
    memcpy(res->name, buffer, responseSize);

    free(buffer);
    return res;
}

void waitForResponse()
{
    int clientFifoHandle = open(CLIENT_FIFO, O_RDONLY);

    int responseSize = 0;
    if (read(clientFifoHandle, &responseSize, sizeof(int)) > 0)
    {
        printf("res size: %i \n", responseSize);
        response* res = receiveResponse(clientFifoHandle, responseSize);
        printf("%s \n", res->name);
    }
}

int main(int argc, char * argv[])
{
    int serverFifoHandle = open(SERVER_FIFO, O_WRONLY);

    request req;
    req.id = atoi(argv[1]);
    req.homepath = getenv("HOME");
    req.payloadSize = sizeof(req.id) + strlen(req.homepath);
    sendRequest(serverFifoHandle, &req);

    waitForResponse();
}
