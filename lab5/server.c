#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CLIENT_FIFO "client_fifo"
#define SERVER_FIFO "server_fifo"

#define DATABASE_RECORDS 20
#define NAME_SIZE 20

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
    strcpy(g_database[0].name, "Smith");
    g_database[1].id = 1;
    strcpy(g_database[1].name, "Johnson");
    g_database[2].id = 2;
    strcpy(g_database[2].name, "Williams");
    g_database[3].id = 3;
    strcpy(g_database[3].name, "Brown");
    g_database[4].id = 4;
    strcpy(g_database[4].name, "Jones");
    g_database[5].id = 5;
    strcpy(g_database[5].name, "Miller");
    g_database[6].id = 6;
    strcpy(g_database[6].name, "Davis");
    g_database[7].id = 7;
    strcpy(g_database[7].name, "Garcia");
    g_database[8].id = 8;
    strcpy(g_database[8].name, "Rodriguez");
    g_database[9].id = 9;
    strcpy(g_database[9].name, "Wilson");
    g_database[10].id = 10;
    strcpy(g_database[10].name, "Martinez");
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

request* receiveRequest(int serverFifoHandle, int requestPayloadSize)
{
    request* req = malloc(sizeof(requestPayloadSize) + requestPayloadSize);
    req->payloadSize = requestPayloadSize;
    req->homepath = malloc(requestPayloadSize - sizeof(req->id));

    void* buffer = malloc(requestPayloadSize);
    read(serverFifoHandle, buffer, requestPayloadSize);

    memcpy(&req->id, buffer, sizeof(req->id));
    memcpy(req->homepath, buffer + sizeof(req->id), requestPayloadSize - sizeof(req->id));

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
    memcpy(buffer + sizeof(res->payloadSize), res->name, res->payloadSize);
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
    int fifo = open(SERVER_FIFO, O_RDONLY);
    while (1)
    {
        int requestPayloadSize = 0;
        if (read(fifo, &requestPayloadSize, sizeof(int)) > 0)
        {
            handleRequest(fifo, requestPayloadSize);
        }
    }
}

void initializeFifo()
{
    mkfifo(SERVER_FIFO, 0666);
    mkfifo(CLIENT_FIFO, 0666);
}

int main()
{
    initializeFifo();
    initializeDatabase();
    waitForRequests();
}
