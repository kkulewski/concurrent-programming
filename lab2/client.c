#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define RETRY_INTERVAL_MS 2000

char MESSAGE[256];

void writeToFile(const char* fileName, const char text[], int characters)
{
  while (open("lockfile", O_CREAT | O_EXCL, 0) == -1)
  {
    printf("Server is busy, please wait...\n");
    usleep(1000 * RETRY_INTERVAL_MS);
  }

  int file = open(fileName, O_CREAT | O_WRONLY | O_APPEND);
  write(file, text, characters);

  // simulate traffic - hold lock a little longer
  usleep(1000 * RETRY_INTERVAL_MS * 5);

  unlink("lockfile");
}

char* getUserInput()
{
  printf("Type:\n");
  scanf("%s", MESSAGE);
  return MESSAGE;
}

void main(void)
{
  const char fileName[] = "data.txt";
  const char* input = getUserInput();

  writeToFile(fileName, input, sizeof(input));
}
