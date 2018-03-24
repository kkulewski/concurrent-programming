#include <stdio.h>

#define SERVER_FIFO "server_fifo"
#define CLIENT_FIFO "client_fifo"

typedef struct record_t
{
    int id;
    char* name;
} record;

int main()
{
    int recordsCount = 3;
    record records[recordsCount];
    for (int i = 0; i < recordsCount; i++)
    {
        records[i].id = i;
    }
    
    records[0].name = "Kowalski";
    records[1].name = "Nowak";
    records[2].name = "Wisniewski";

    for (int i = 0; i < recordsCount; i++)
    {
        printf("[%i] %s \n", records[i].id, records[i].name);
    }
}
