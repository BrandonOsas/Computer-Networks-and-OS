#include <syscall.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  create(argv[1], 20);
  printf("file has been created!!\n\n\n");
  return 0;
}
