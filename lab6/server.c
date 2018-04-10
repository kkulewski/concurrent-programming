#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>

#define CLIENT_KEY 333
#define SERVER_KEY 666

#define WORD_SIZE 256
#define DICTIONARY_SIZE 3

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

entry dictionary[DICTIONARY_SIZE] =
{
   { "czerwony", "red" },
   { "zielony", "green" },
   { "niebieski", "blue" }
};

int g_clientQueue;
int g_serverQueue;

void cleanup(int signal)
{
    msgctl(g_clientQueue, IPC_RMID, 0);
    msgctl(g_serverQueue, IPC_RMID, 0);
    exit(0);
}

char* translate(char* word)
{
    for (int i = 0; i < DICTIONARY_SIZE; i++)
    {
        if (strcmp(word, dictionary[i].pol) == 0)
            return dictionary[i].eng;
    }
    return "[not found]";
}

int main()
{
    // trap SIGINT (Ctrl+C) to run cleanup
    signal(SIGINT, cleanup);

    g_clientQueue = msgget(CLIENT_KEY, 0666 | IPC_CREAT);
    g_serverQueue = msgget(SERVER_KEY, 0666 | IPC_CREAT);

    message msg;
    while (1)
    {
        msgrcv(g_serverQueue, &msg, sizeof(char) * WORD_SIZE, 0, 0);
        strcpy(msg.word, translate(msg.word));
        msgsnd(g_clientQueue, &msg, sizeof(char) * WORD_SIZE, 0);
    }

    return 0;
}
