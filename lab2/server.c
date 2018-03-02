#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE_BYTES 1024

#define CLIENT_MESSAGE_FILE "client/data.txt"
#define CLIENT_LOCK_FILE "client/lockfile"
#define SERVER_MESSAGE_FILE "server/data.txt"
#define SERVER_LOCK_FILE "server/lockfile"


void writeTextToFile(const char* fileName, char* text, int bytes)
{
  int file = open(fileName, O_CREAT | O_WRONLY | O_APPEND);
  write(file, text, bytes);
}

char* readTextFromFile(const char* fileName, int bytes)
{
  int file = open(fileName, O_RDONLY);
  char* responseBuffer = malloc(bytes);
  read(file, responseBuffer, bytes);
  return responseBuffer;
}

void main(void)
{
  while(1)
  {
    if ( access(CLIENT_LOCK_FILE, F_OK) != -1 )
    {
      usleep(1000 * 1000);
      unlink(CLIENT_LOCK_FILE);
      char* clientMessage = readTextFromFile(CLIENT_MESSAGE_FILE, BUFFER_SIZE_BYTES);
      unlink(SERVER_MESSAGE_FILE);
      writeTextToFile(SERVER_MESSAGE_FILE, clientMessage, BUFFER_SIZE_BYTES);
    }
  }
}
