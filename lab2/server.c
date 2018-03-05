#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  // SET FILEPATHs
  char* clientName_FilePath_OnServer = getFullFilePath(serverName, "clientName");
  char* clientNameLock_FilePath_OnServer = getFullFilePath(serverName, "clientName.lock");
  char* clientMessage_FilePath_OnServer = getFullFilePath(serverName, "clientMessage");
  char* clientMessageLock_FilePath_OnServer = getFullFilePath(serverName, "clientMessage.lock");

  printf("### SERVER IS RUNNING... ###\n");
  while(1)
  {
    // WAIT FOR CLIENT ADDRESS LOCK
    if ( access(clientNameLock_FilePath_OnServer, F_OK) != -1 )
    {
      printf("\n### CLIENT CONNECTED ###\n");

      // READ CLIENT ADDRESS
      char* clientName = readTextFromFile(clientName_FilePath_OnServer, BUFFER_SIZE_BYTES);
      printf("[ADDRESS ]:\n%s\n", clientName);

      // WAIT FOR CLIENT MESSAGE LOCK
      while ( access(clientMessageLock_FilePath_OnServer, F_OK) == -1 )
      {
      }

      // READ CLIENT MESSAGE
      char* clientMessage = readTextFromFile(clientMessage_FilePath_OnServer, BUFFER_SIZE_BYTES);
      printf("[MESSAGE ]:\n%s\n", clientMessage);

      // SEND RESPONSE TO CLIENT
      printf("[RESPONSE]:\n");
      char* serverResponse = getUserInput(BUFFER_SIZE_BYTES);
      char* serverResponse_FilePath_OnClient = getFullFilePath(clientName, "response");
      writeTextToFile(serverResponse_FilePath_OnClient, serverResponse, BUFFER_SIZE_BYTES);


      // DESTROY CLIENT ADDRESS, MESSAGE & LOCKs
      remove(clientNameLock_FilePath_OnServer);
      remove(clientName_FilePath_OnServer);
      remove(clientMessageLock_FilePath_OnServer);
      remove(clientMessage_FilePath_OnServer);
      printf("### CONVERSATION FINISHED ###\n");
    }
  }
}
