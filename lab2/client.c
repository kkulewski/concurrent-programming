#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define SLEEP_MILISECONDS 500

void writeToFile(const char* fileName, const char text[], int characters)
{
  int fileHandle;
  while (fileHandle = open(fileName, O_CREAT | O_WRONLY | O_APPEND) == -1)
  {
      printf("Server is busy, please wait...");
      usleep(SLEEP_MILISECONDS * 1000);
  }

  flock(fileHandle, LOCK_EX);
  write(fileHandle, text, characters);
  flock(fileHandle, LOCK_UN);
}

char* getUserInput()
{
  printf("Type:\n");
  static char message[256];
  scanf("%s", message);
  return message;
}

void main(void)
{
  const char fileName[] = "data.txt";
  const char* input = getUserInput();

  writeToFile(fileName, input, sizeof(input));
}
