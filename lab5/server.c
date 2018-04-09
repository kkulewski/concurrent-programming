#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CLIENT_FIFO "client_fifo"
#define DATABASE_RECORDS 3
#define NAME_SIZE 20

typedef struct record_t
{
    int id;
    char* name;

} record;

typedef struct request_t
{
    int id;
    char* homepath;

} request;

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

char* getRecordById(int recordId)
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

request* receiveRequest(int fifoHandle, int requestSize)
{
    void* buffer = malloc(requestSize);
    request* req = malloc(requestSize);
    req->homepath = malloc(requestSize - sizeof(req->id));

    read(fifoHandle, buffer, requestSize);
    memcpy(&req->id, buffer, sizeof(req->id));
    memcpy(req->homepath, buffer + sizeof(req->id), requestSize - sizeof(req->id));

    free(buffer);
    return req;
}

void handleRequest(request* req)
{
    printf("## Received request from: %s \n", req->homepath);
    printf("- requested record ID: %i \n", req->id);
    printf("- requested content  : %s \n", getRecordById(req->id));
    printf("\n");
}

void waitForRequests(int fifoHandle)
{
    while (1)
    {
        int requestSize = 0;
        if (read(fifoHandle, &requestSize, sizeof(int)) > 0)
        {
            request* req = receiveRequest(fifoHandle, requestSize);
            handleRequest(req);
        }
    }
}

int main()
{
    initializeDatabase();

    mkfifo(CLIENT_FIFO, 0666);
    int fifoHandle = open(CLIENT_FIFO, O_RDONLY);

    waitForRequests(fifoHandle);
}
