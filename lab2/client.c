#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETRY_INTERVAL_MS 2000
#define BUFFER_SIZE_BYTES 1024

char* getFullFilePath(char* user, char* fileName)
{
  char* path = malloc(BUFFER_SIZE_BYTES);
  strcat(path, "/home/");
  strcat(path, user);
  strcat(path, "/cs/");
  strcat(path, fileName);
  return path;
}

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
  char* buffer = malloc(bytes);
  char* line = malloc(bytes);

  size_t len = 0;
  while (getline(&line, &len, stdin) > 0)
  {
    strcat(buffer, line);
    line = malloc(bytes);
  }

  buffer[strlen(buffer) - 1] = '\0';
  return buffer;
}

void main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error - provide server name.\n");
    return;
  }

  char* serverName = argv[1];

  // GET CLIENT ADDRESS/NAME
  char* clientName_FilePath_OnServer = getFullFilePath(serverName, "clientName");
  char* clientNameLock_FilePath_OnServer = getFullFilePath(serverName, "clientName.lock");
  printf("[YOUR ADDRESS]:\n");
  char* clientName = getUserInput(BUFFER_SIZE_BYTES);

  // GET CLIENT MESSAGE
  char* clientMessage_FilePath_OnServer = getFullFilePath(serverName, "clientMessage");
  char* clientMessageLock_FilePath_OnServer = getFullFilePath(serverName, "clientMessage.lock");
  printf("[YOUR MESSAGE]:\n");
  char* clientMessage = getUserInput(BUFFER_SIZE_BYTES);

  // SEND DATA TO SERVER
  waitForLockFile(clientNameLock_FilePath_OnServer);
  writeTextToFile(clientName_FilePath_OnServer, clientName, BUFFER_SIZE_BYTES);
  waitForLockFile(clientMessageLock_FilePath_OnServer);
  writeTextToFile(clientMessage_FilePath_OnServer, clientMessage, BUFFER_SIZE_BYTES);

  // SERVER RESPONSE
  char* serverResponse_FilePath_OnClient = getFullFilePath(clientName, "response");

  // WAIT FOR RESPONSE
  printf("[WAIT...]\n");
  while ( access(serverResponse_FilePath_OnClient, F_OK) == -1 )
  {
  }

  // READ RESPONSE
  printf("[RESPONSE]:\n");
  char* response = readTextFromFile(serverResponse_FilePath_OnClient, BUFFER_SIZE_BYTES);
  printf("%s\n", response);
  remove(serverResponse_FilePath_OnClient);
}
