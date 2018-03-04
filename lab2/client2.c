#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETRY_INTERVAL_MS 1000
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

void main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error - provide server address.\n");
    return;
  }

  char* serverAddress = argv[1];

  // PREPARE PATHS + GET USER DATA
  // CLIENT ADDRESS
  char* clientAddressLockPath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientAddressLockPath, "/home/");
  strcat(clientAddressLockPath, serverAddress);
  strcat(clientAddressLockPath, "/cs/clientAddress.lock");

  char* clientAddressFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientAddressFilePath, "/home/");
  strcat(clientAddressFilePath, serverAddress);
  strcat(clientAddressFilePath, "/cs/clientAddress");

  printf("Please enter your ID/address:\n");
  char* clientAddress = getUserInput(BUFFER_SIZE_BYTES);

  // SEND CLIENT MESSAGE TO SERVER
  // CLIENT MESSAGE
  char* clientMessageLockPath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientMessageLockPath, "/home/");
  strcat(clientMessageLockPath, serverAddress);
  strcat(clientMessageLockPath, "/cs/clientMessage.lock");

  char* clientMessageFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(clientMessageFilePath, "/home/");
  strcat(clientMessageFilePath, serverAddress);
  strcat(clientMessageFilePath, "/cs/clientMessage");

  printf("Please enter your message:\n");
  char* clientMessage = getUserInput(BUFFER_SIZE_BYTES);

  // SEND DATA TO SERVER
  waitForLockFile(clientAddressLockPath);
  writeTextToFile(clientAddressFilePath, clientAddress, BUFFER_SIZE_BYTES);
  waitForLockFile(clientMessageLockPath);
  writeTextToFile(clientMessageFilePath, clientMessage, BUFFER_SIZE_BYTES);


  // SERVER RESPONSE
  char* serverResponseFilePath = malloc(BUFFER_SIZE_BYTES);
  strcat(serverResponseFilePath, "/home/");
  strcat(serverResponseFilePath, clientAddress);
  strcat(serverResponseFilePath, "/cs/response");

  // WAIT FOR RESPONSE
  while ( access(serverResponseFilePath, F_OK) == -1 )
  {
  }

  // READ RESPONSE
  printf("Server response:\n");
  char* response = readTextFromFile(serverResponseFilePath, BUFFER_SIZE_BYTES);
  printf("%s\n", response);
  remove(serverResponseFilePath);
}
