#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <filesys/filesys.h>

int main(int argc, char *argv[])
{
  remove(argv[1]);
  printf("file has been removed!!\n\n\n");
  return 0;
}
