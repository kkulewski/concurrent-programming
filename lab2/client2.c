#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETRY_INTERVAL_MS 2000
#define BUFFER_SIZE_BYTES 1024

void waitForLockFile(const char* fileName)
{
  while (open(fileName, O_CREAT | O_EXCL, 0) == -1)
  {
    printf("Server is busy, please wait...\n");
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

char* getFullFilePath(char* user, char* fileName)
{
  char* path = malloc(BUFFER_SIZE_BYTES);
  strcat(path, "/home/");
  strcat(path, user);
  strcat(path, "/cs/");
  strcat(path, fileName);
  return path;
}

void main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error - provide server address.\n");
    return;
  }

  char* serverAddress = argv[1];

  // GET CLIENT ADDRESS
  char* clientAddressLockPath = getFullFilePath(serverAddress, "clientAddress.lock");
  char* clientAddressFilePath = getFullFilePath(serverAddress, "clientAddress");
  printf("\nPlease enter your ID/address:\n");
  char* clientAddress = getUserInput(BUFFER_SIZE_BYTES);

  // GET CLIENT MESSAGE
  char* clientMessageLockPath = getFullFilePath(serverAddress, "clientMessage.lock");
  char* clientMessageFilePath = getFullFilePath(serverAddress, "clientMessage");
  printf("Please enter your message:\n");
  char* clientMessage = getUserInput(BUFFER_SIZE_BYTES);

  // SEND DATA TO SERVER
  waitForLockFile(clientAddressLockPath);
  writeTextToFile(clientAddressFilePath, clientAddress, BUFFER_SIZE_BYTES);
  waitForLockFile(clientMessageLockPath);
  writeTextToFile(clientMessageFilePath, clientMessage, BUFFER_SIZE_BYTES);

  // SERVER RESPONSE
  char* serverResponseFilePath = getFullFilePath(clientAddress, "response");

  // WAIT FOR RESPONSE
  while ( access(serverResponseFilePath, F_OK) == -1 )
  {
  }

  // READ RESPONSE
  printf("\nServer response:\n");
  char* response = readTextFromFile(serverResponseFilePath, BUFFER_SIZE_BYTES);
  printf("%s\n\n", response);
  remove(serverResponseFilePath);
}
