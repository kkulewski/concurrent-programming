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

  // SET FILEPATHs
  char* serverAddress = argv[1];
  char* clientAddressLockPath = getFullFilePath(serverAddress, "clientAddress.lock");
  char* clientAddressFilePath = getFullFilePath(serverAddress, "clientAddress");
  char* clientMessageLockPath = getFullFilePath(serverAddress, "clientMessage.lock");
  char* clientMessageFilePath = getFullFilePath(serverAddress, "clientMessage");

  printf("### SERVER IS RUNNING... ###\n");
  while(1)
  {
    // WAIT FOR CLIENT ADDRESS LOCK
    if ( access(clientAddressLockPath, F_OK) != -1 )
    {
      printf("\n### CLIENT CONNECTED ###\n");

      // READ CLIENT ADDRESS
      char* clientAddress = readTextFromFile(clientAddressFilePath, BUFFER_SIZE_BYTES);
      printf("[ADR]: %s\n", clientAddress);

      // WAIT FOR CLIENT MESSAGE LOCK
      while ( access(clientMessageLockPath, F_OK) == -1 )
      {
      }

      // READ CLIENT MESSAGE
      char* clientMessage = readTextFromFile(clientMessageFilePath, BUFFER_SIZE_BYTES);
      printf("[MSG]: %s\n", clientMessage);

      // SEND RESPONSE TO CLIENT
      printf("[RSP]: ");
      char* serverResponse = getUserInput(BUFFER_SIZE_BYTES);
      char* responseAddressFilePath = getFullFilePath(clientAddress, "response");
      writeTextToFile(responseAddressFilePath, serverResponse, BUFFER_SIZE_BYTES);

      // DESTROY CLIENT ADDRESS, MESSAGE & LOCKs
      remove(clientAddressFilePath);
      remove(clientAddressLockPath);
      remove(clientMessageFilePath);
      remove(clientMessageLockPath);

      printf("### CONVERSATION FINISHED ###\n\n");
    }
  }
}
