#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include <user/syscall.h>
#include "threads/vaddr.h"
#include "threads/init.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include <list.h>

#define ARG_CODE 0
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12
//#endif

typedef int pid_t;
struct lock filesys_lock;

static void syscall_handler (struct intr_frame *f);

void syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static uint32_t load_stack(struct intr_frame *f, int offset)
{
  return *((uint32_t*)(f->esp + offset));
}

static int handle_write(int fd, const void * buffer, unsigned int length)
{
 if (fd == STDOUT_FILENO)
  {
      putbuf((const char *)buffer, (size_t)length);
  }

  else
  {
    printf("handle_write does not support fd output\n");
  }

    return length;
}

static void syscall_handler (struct intr_frame *f){
  int code = (int)load_stack(f, ARG_CODE);

  switch (code) {
   case SYS_WRITE:
   {
     int result = handle_write(
       (int)load_stack(f,ARG_1),
       (void *)load_stack(f, ARG_2),
       (unsigned int)load_stack(f, ARG_3));

       f->eax = result;
       break;
     }

   case SYS_HALT:
   {
     halt();
     break;
   }

   case SYS_EXIT:
   {
    exit(
    (int)load_stack(f, ARG_1));
    break;
   }

   case SYS_EXEC:
   {
    //  printf("exec not implementedz\n");
    //  break;
   }

   case SYS_WAIT:
   {
    // printf("wait not implemented\n");
    // break;
   }

   case SYS_CREATE:
   {
    int result = create(
    (char *)load_stack(f, ARG_1),
    (unsigned) load_stack(f, ARG_2));

    f->eax = result;
    break;
   }

   case SYS_REMOVE:
   {
    int result = remove(
    (char *) load_stack(f,ARG_1));

    f->eax = result;
    break;
   }

   case SYS_OPEN:
   {
     int result = open(
     (char *)load_stack(f, ARG_1));

     f->eax = result;
     break;
   }

   case SYS_FILESIZE:
   {
    // f->eax = filesize((int) load_stack(f, ARG_1));
    // break;
   }

   case SYS_READ:
   {
     //  printf("read not implemented\n");
     //  break;
   }

   case SYS_SEEK:
   {
    //  printf("seek not implemented\n");
    //  break;
   }

   case SYS_TELL:
   {
    //  printf("tell not implemented\n");
    //  break;
   }

   case SYS_CLOSE:
   {
    //  printf("close not implemented\n");
    //  break;
   }

   default:
     printf("SYS_CALL (%d) not implemented\n", code);
     thread_exit ();
    }
   }

   //HALTING SYSTEM CALL
   //Terminates Pintos by calling shutdown_power_off()

    void halt(void){
     shutdown_power_off();
   }

   //EXIT SYSTEM CALL
   //Terminates the current user program, returning status to the kernel
   //A status of 0 will indicate success and a nonzero value indicates error

   void exit(int status){
     struct thread * current = thread_current();
     current->exit_status = status;
     thread_exit();
   }

   //OPEN SYSTEM CALL
   //Opens the file specified.
   //Will return 'file has been opened' if successfull
   //Needs validation

   int open (const char *file){
     struct thread *current = thread_current();
     struct file_struct *file_mem = malloc(sizeof(struct file_struct));

     file_mem->open_file = filesys_open(file);
     file_mem->fd = current->current_fd;

     current->current_fd += 1;
     list_push_back(&current->files, &file_mem->elem);
     return file_mem->fd;
   }

   //CREATE DOCUMENT
   //Creates a new file that could be viewed in 'pintos ls'
   //Will return 'file has been created' if successful

   bool create (const char *file, unsigned initial_size)
   {
     return filesys_create(file, (off_t) initial_size);
   }

   //REMOVE DOCUMENT
   //Removes documents specified
   //Will return 'file has been deleted' if successful
   //File may be removed even if its open

   bool remove (const char *file){
     return filesys_remove(file);
   }
