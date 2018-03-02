#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE_BYTES 1024

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

void main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error - provide server address.\n");
    return;
  }

  // OBTAIN SERVER ADDRESS
  char* serverAddress = argv[1];
  char* serverAddressFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(serverAddressFilePath, "/home/");
  strcat(serverAddressFilePath, serverAddress);
  strcat(serverAddressFilePath, "/cs/s/");

  // CLIENT LOCK FILEPATH
  char* clientLockFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientLockFilePath, serverAddressFilePath);
  strcat(clientLockFilePath, "lockfile");

  while(1)
  {
    // WAIT FOR NEW CLIENT LOCK
    if ( access(clientLockFilePath, F_OK) != -1 )
    {

      // READ CLIENT ADDRESS
      char* clientAddressFilePath = malloc(BUFFER_SIZE_BYTES);
      strcat(clientAddressFilePath, "clientAddress");
      char* clientAddress = readTextFromFile(clientAddressFilePath, BUFFER_SIZE_BYTES);
      printf("[ADR]: %s\n", clientAddress);

      // READ CLIENT MESSAGE
      char* clientMessageFilePath = malloc(BUFFER_SIZE_BYTES);
      strcat(clientMessageFilePath, "clientMessage");
      char* clientMessage = readTextFromFile(clientMessageFilePath, BUFFER_SIZE_BYTES);
      printf("[MSG]: %s\n", clientMessage);

      // SEND RESPONSE TO CLIENT ADDRESS
      char* responseAddressFilePath = malloc(BUFFER_SIZE_BYTES);
      strcat(responseAddressFilePath, "/home/");
      strcat(responseAddressFilePath, clientAddress);
      strcat(responseAddressFilePath, "/cs/c/response");
      writeTextToFile(responseAddressFilePath, "Hello dear client!", BUFFER_SIZE_BYTES);

      // DESTROY CLIENT ADDRESS, MESSAGE & LOCK
      remove(clientAddressFilePath);
      remove(clientMessageFilePath);
      unlink(clientLockFilePath);
    }
  }
}
