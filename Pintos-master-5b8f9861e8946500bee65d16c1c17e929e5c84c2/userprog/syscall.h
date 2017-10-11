#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "threads/thread.h"
#include "threads/interrupt.h"

typedef int pid_t;

void syscall_init (void);
void halt (void);                   //DONE
void exit (int status);             //DONE
pid_t exec (const char *file);
int wait (pid_t pid);
bool create (const char *file, unsigned initial_size);      //DONE
bool remove (const char *file);                             //DONE
int open (const char *file);                                //DONE
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

#endif /* userprog/syscall.h */
