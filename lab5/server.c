#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
    char* home;
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

int main()
{
    record* db = getDatabase();

    printf("%s\n", getRecordById(db, 0));
    printf("%s\n", getRecordById(db, 1));
    printf("%s\n", getRecordById(db, 2));
    printf("%s\n", getRecordById(db, 3));
}
