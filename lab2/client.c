#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define RETRY_INTERVAL_MS 2000
#define BUFFER_SIZE_BYTES 1024
#define DATA_FILE_NAME "data.txt"

void waitForFile()
{
  while (open("lockfile", O_CREAT | O_EXCL, 0) == -1)
  {
    printf("[CLIENT] Server is busy, please wait...\n");
    usleep(1000 * RETRY_INTERVAL_MS);
  }
}

void writeTextToFile(const char* fileName, char* text, int bytes)
{
  int file = open(fileName, O_CREAT | O_WRONLY | O_APPEND);
  write(file, text, bytes);

  // simulate traffic - hold lock a little longer
  usleep(1000 * RETRY_INTERVAL_MS * 5);
  unlink("lockfile");
}

char* readTextFromFile(const char* fileName)
{
    int file = open(fileName, O_RDONLY);
    char* responseBuffer = malloc( BUFFER_SIZE_BYTES );
    read(file, responseBuffer, BUFFER_SIZE_BYTES);
    return responseBuffer;
}

char* getUserInput()
{
  char* messageBuffer = malloc( BUFFER_SIZE_BYTES );
  scanf("%s", messageBuffer);
  return messageBuffer;
}

void main(void)
{
  printf("[CLIENT] Enter the message:\n");
  char* input = getUserInput();

  printf("[CLIENT] Waiting for server...\n");
  waitForFile();

  printf("[CLIENT] Sending to server...\n");
  writeTextToFile(DATA_FILE_NAME, input, BUFFER_SIZE_BYTES);

  printf("[CLIENT] Server response:\n");
  char* response = readTextFromFile(DATA_FILE_NAME);
  printf("%s\n", response);
}
