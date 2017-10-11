#include <syscall.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  open(argv[1]);
  printf("'%s' ", argv[1]);
  printf("file has been opened!\n");

  return 0;
}
