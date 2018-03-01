#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void writeToFile(const char* fileName, const char text[], int characters)
{
  int fileHandle = open(fileName, O_CREAT | O_WRONLY | O_APPEND);
  write(fileHandle, text, characters);
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
