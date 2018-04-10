#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define CLIENT_KEY 333
#define SERVER_KEY 666

#define WORD_SIZE 256

typedef struct entry_t
{
    char pol[WORD_SIZE];
    char eng[WORD_SIZE];
} entry;

typedef struct message_t
{
    long type;
    char word[WORD_SIZE];
} message;

int main(int argc, char* argv[])
{
    int clientQueue = msgget(CLIENT_KEY, 0666);
    int serverQueue = msgget(SERVER_KEY, 0666);

    message msg;
    strcpy(msg.word, argv[1]);
    msg.type = 1;

    msgsnd(serverQueue, &msg, sizeof(char) * WORD_SIZE, 0);
    msgrcv(clientQueue, &msg, sizeof(char) * WORD_SIZE, 0, 0);

    printf("POL: %s\n", argv[1]);
    printf("ENG: %s\n", msg.word);

    return 0;
}
