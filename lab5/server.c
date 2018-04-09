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

request receiveRequest(int fifoHandle, int requestSize)
{
    void* buffer = malloc(requestSize);
    read(fifoHandle, buffer, requestSize);

    request req;
    req.homepath = malloc(requestSize - sizeof(req.id));
    memcpy(&req.id, buffer, sizeof(req.id));
    memcpy(req.homepath, buffer + sizeof(req.id), requestSize - sizeof(req.id));

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

void waitForRequests(int fifoHandle, record* recordsDatabase)
{
    while (1)
    {
        int requestLength = 0;
        if (read(fifoHandle, &requestLength, sizeof(int)) > 0)
        {
            request req = receiveRequest(fifoHandle, requestLength);
            handleRequest(&req, recordsDatabase);
        }
    }
}

int main()
{
    record* recordsDatabase = getDatabase();

    mkfifo(CLIENT_FIFO, 0666);
    int fifoHandle = open(CLIENT_FIFO, O_RDONLY);

    waitForRequests(fifoHandle, recordsDatabase);
}
