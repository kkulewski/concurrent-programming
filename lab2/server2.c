#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETRY_INTERVAL_MS 1000
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
  strcat(serverAddressFilePath, "/cs/");

  // CLIENT ADDRESS LOCK FILEPATH
  char* clientAddressLockPath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientAddressLockPath, serverAddressFilePath);
  strcat(clientAddressLockPath, "clientAddress.lock");

  // CLIENT ADDRESS FILEPATH
  char* clientAddressFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientAddressFilePath, serverAddressFilePath);
  strcat(clientAddressFilePath, "clientAddress");

  // CLIENT MESSAGE LOCK FILEPATH
  char* clientMessageLockPath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientMessageLockPath, serverAddressFilePath);
  strcat(clientMessageLockPath, "clientMessage.lock");

  // CLIENT MESSAGE FILEPATH
  char* clientMessageFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientMessageFilePath, serverAddressFilePath);
  strcat(clientMessageFilePath, "clientMessage");

  while(1)
  {
    // WAIT FOR CLIENT ADDRESS LOCK
    if ( access(clientAddressLockPath, F_OK) != -1 )
    {
      printf("### CLIENT CONNECTED ###\n");

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
      char* responseAddressFilePath = malloc(BUFFER_SIZE_BYTES);
      strcat(responseAddressFilePath, "/home/");
      strcat(responseAddressFilePath, clientAddress);
      strcat(responseAddressFilePath, "/cs/response");
      printf("Please enter your response:\n");

      char* serverResponse = getUserInput(BUFFER_SIZE_BYTES);
      writeTextToFile(responseAddressFilePath, serverResponse, BUFFER_SIZE_BYTES);

      // DESTROY CLIENT ADDRESS, MESSAGE & LOCK
      remove(clientAddressFilePath);
      remove(clientAddressLockPath);
      remove(clientMessageFilePath);
      remove(clientMessageLockPath);

      printf("### CONVERSATION FINISHED ###\n\n");
    }
  }
}
