#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define RETRY_INTERVAL_MS 2000
#define BUFFER_SIZE_BYTES 1024

#define CLIENT_MESSAGE_FILE "client/data.txt"
#define CLIENT_LOCK_FILE "client/lockfile"
#define SERVER_MESSAGE_FILE "server/data.txt"
#define SERVER_LOCK_FILE "server/lockfile"

void waitForFile()
{
  while (open(CLIENT_LOCK_FILE, O_CREAT | O_EXCL, 0) == -1)
  {
    printf("[CLIENT] Server is busy, please wait...\n");
    usleep(1000 * RETRY_INTERVAL_MS);
  }
}

void writeTextToFile(const char* fileName, char* text, int bytes)
{
  int file = open(fileName, O_CREAT | O_WRONLY | O_APPEND);
  write(file, text, bytes);
}

char* readTextFromFile(const char* fileName, int bytes)
{
  int file = open(fileName, O_RDONLY);
  char* buffer = malloc( BUFFER_SIZE_BYTES );
  read(file, buffer, BUFFER_SIZE_BYTES);
  return buffer;
}

char* getUserInput(int bytes)
{
  char* buffer = malloc( bytes );
  scanf("%s", buffer);
  return buffer;
}

void main(void)
{
  printf("[CLIENT] Enter the message:\n");
  char* clientMessage = getUserInput(BUFFER_SIZE_BYTES);

  printf("[CLIENT] Waiting for server...\n");
  waitForFile();

  printf("[CLIENT] Sending to server...\n");
  writeTextToFile(CLIENT_MESSAGE_FILE, clientMessage, BUFFER_SIZE_BYTES);

  printf("[CLIENT] Server response:\n");
  usleep(1000 * RETRY_INTERVAL_MS);
  char* response = readTextFromFile(SERVER_MESSAGE_FILE, BUFFER_SIZE_BYTES);
  printf("%s\n", response);
}
