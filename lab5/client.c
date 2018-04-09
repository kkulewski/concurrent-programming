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
    int payloadSize;

    // payload
    char* name;

} response;

request* createRequest(int id, char* homepath)
{
    request* req = malloc(sizeof(request));
    req->id = id;
    req->homepath = homepath;
    req->payloadSize = sizeof(req->id) + strlen(req->homepath);
    return req;
}

void sendRequest(int serverFifoHandle, request* req)
{
    int requestSize = sizeof(req->payloadSize) + req->payloadSize;
    void* buffer = malloc(requestSize);

    memcpy(buffer, &req->payloadSize, sizeof(req->payloadSize));
    memcpy(buffer + sizeof(req->payloadSize), &req->id, sizeof(req->id));
    memcpy(buffer + sizeof(req->payloadSize) + sizeof(req->id), req->homepath, req->payloadSize - sizeof(req->id));

    write(serverFifoHandle, buffer, requestSize);
    free(buffer);
}

response* receiveResponse(int clientFifoHandle, int responsePayloadSize)
{
    response* res = malloc(sizeof(responsePayloadSize) + responsePayloadSize);
    res->payloadSize = responsePayloadSize;
    res->name = malloc(responsePayloadSize);

    void* buffer = malloc(responsePayloadSize);
    read(clientFifoHandle, buffer, responsePayloadSize);
    memcpy(res->name, buffer, responsePayloadSize);

    free(buffer);
    return res;
}

void handleResponse(int clientFifoHandle)
{
    int responsePayloadSize = 0;
    read(clientFifoHandle, &responsePayloadSize, sizeof(responsePayloadSize));
    response* res = receiveResponse(clientFifoHandle, responsePayloadSize);
    printf("%s\n", res->name);
}

int main(int argc, char * argv[])
{
    int serverFifo = open(SERVER_FIFO, O_WRONLY);
    request* req = createRequest(atoi(argv[1]), getenv("HOME"));
    sendRequest(serverFifo, req);

    int clientFifo = open(CLIENT_FIFO, O_RDONLY);
    handleResponse(clientFifo);
}
