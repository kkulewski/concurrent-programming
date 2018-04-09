#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SERVER_FIFO "server_fifo"
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

record* getDatabase()
{
    record* records = malloc(DATABASE_RECORDS * sizeof(record));
    for (int i = 0; i < DATABASE_RECORDS; i++)
    {
        records[i].id = -1;
        records[i].name = malloc(NAME_SIZE * sizeof(char));
    }

    records[0].id = 0;
    strcpy(records[0].name, "Kowalski");
    records[1].id = 1;
    strcpy(records[1].name, "Nowak");
    records[2].id = 2;
    strcpy(records[2].name, "Wisniewski");

    return records;
}

char* getRecordById(record* records, int recordId)
{
    for (int i = 0; i < DATABASE_RECORDS; i++)
    {
        if (records[i].id == recordId)
        {
            return records[i].name;
        }
    }

    return "Not found!";
}

request getRequest(int fifoHandle, int requestLength)
{
    unsigned char* buffer = (char*) malloc(requestLength);
    read(fifoHandle, buffer, requestLength);

    request req;
    memcpy(&req.id, buffer, sizeof(req->id));
    req.homepath = (char*) malloc(requestLength - sizeof(int));
    memcpy(req.homepath, buffer + sizeof(req->id), requestLength - sizeof(req->id));

    free(buffer);
    return req;
}

void handleRequest(request* req, record* recordsDatabase)
{
    printf("## Received request from: %s \n", req->homepath);
    printf("- requested record ID: %i \n", req->id);
    printf("- requested content  : %s \n", getRecordById(recordsDatabase, req->id));
    printf("\n");
}

int main()
{
    record* recordsDatabase = getDatabase();

    mkfifo(CLIENT_FIFO, 0666);
    int clientFifoHandle = open(CLIENT_FIFO, O_RDONLY);

    request req;
    int requestLength = 0;
    int bytesRead = 0;

    while (1)
    {
        if ((bytesRead = read(clientFifoHandle, &requestLength, sizeof(int))) > 0)
        {
            req = getRequest(clientFifoHandle, requestLength);
            handleRequest(&req, recordsDatabase);
        }
    }
}
