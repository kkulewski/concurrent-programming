#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CLIENT_FIFO "client_fifo"
#define SERVER_FIFO "server_fifo"

#define DATABASE_RECORDS 3
#define NAME_SIZE 20

typedef struct request_t
{
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

typedef struct record_t
{
    int id;
    char* name;

} record;

record* g_database;

void initializeDatabase()
{
    g_database = malloc(DATABASE_RECORDS * sizeof(record));
    for (int i = 0; i < DATABASE_RECORDS; i++)
    {
        g_database[i].id = -1;
        g_database[i].name = malloc(NAME_SIZE * sizeof(char));
    }

    g_database[0].id = 0;
    strcpy(g_database[0].name, "Kowalski");
    g_database[1].id = 1;
    strcpy(g_database[1].name, "Nowak");
    g_database[2].id = 2;
    strcpy(g_database[2].name, "Wisniewski");
}

char* getNameByRecordId(int recordId)
{
    for (int i = 0; i < DATABASE_RECORDS; i++)
    {
        if (g_database[i].id == recordId)
        {
            return g_database[i].name;
        }
    }

    return "Not found!";
}

request* receiveRequest(int serverFifoHandle, int requestSize)
{
    void* buffer = malloc(requestSize);
    request* req = malloc(requestSize);
    req->homepath = malloc(requestSize - sizeof(req->id));

    read(serverFifoHandle, buffer, requestSize);
    memcpy(&req->id, buffer, sizeof(req->id));
    memcpy(req->homepath, buffer + sizeof(req->id), requestSize - sizeof(req->id));

    free(buffer);
    return req;
}

response* createResponseForRequest(request* req)
{
    response* resp = malloc(sizeof(response));
    resp->name = getNameByRecordId(req->id);
    resp->payloadSize = strlen(resp->name);

    return resp;
}

void sendResponse(int clientFifoHandle, response* res)
{
    int responseSize = res->payloadSize + sizeof(res->payloadSize);
    void* buffer = malloc(responseSize);

    memcpy(buffer, &res->payloadSize, sizeof(res->payloadSize));
    memcpy(buffer + sizeof(res->payloadSize), &res->name, res->payloadSize);
    write(clientFifoHandle, buffer, responseSize);

    free(buffer);
}

void handleRequest(int serverFifoHandle, int requestSize)
{
    request* req = receiveRequest(serverFifoHandle, requestSize);
    response* res = createResponseForRequest(req);

    // using local client fifo path for sake of simplicity
    int clientFifoHandle = open(CLIENT_FIFO, O_WRONLY);
    sendResponse(clientFifoHandle, res);
    close(clientFifoHandle);
}

void waitForRequests()
{
    int serverFifoHandle = open(SERVER_FIFO, O_RDONLY);

    while (1)
    {
        int requestSize = 0;
        if (read(serverFifoHandle, &requestSize, sizeof(int)) > 0)
        {
            handleRequest(serverFifoHandle, requestSize);
        }
    }
}

int main()
{
    mkfifo(SERVER_FIFO, 0666);
    mkfifo(CLIENT_FIFO, 0666);

    initializeDatabase();
    waitForRequests();
}
