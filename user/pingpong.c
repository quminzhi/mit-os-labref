#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// read and write ends of a pipe
#define READ 0
#define WRITE 1

/**
 * An implementation of pingpong, where a parent process send a byte to its
 * child process through a pipe between them, and the child send one byte back
 * and exit.
 */

int main(int argc, char *argv[]) { 
  int p[2];
  int pid;
  char buf;

  pipe(p);

  pid = fork();
  if (pid == 0) {
    if (read(p[READ], &buf, 1) != 1) {
      fprintf(2, "%d: failed to receive a byte from a child process.\n", getpid());
      close(p[READ]);
      close(p[WRITE]);
      exit(1);
    }
    fprintf(1, "%d: received ping\n", getpid());
    write(p[WRITE], &buf, 1);

    close(p[READ]);
    close(p[WRITE]);
    exit(0);
  }

  write(p[WRITE], "0xFF", 1);
  close(p[WRITE]);

  // reap any exited child process
  wait((int *)0);
  
  // hang over when nothing to read in the pipe
  if (read(p[READ], &buf, 1) != 1) {
    fprintf(2, "%d: failed to receive a byte from a child process.\n", getpid());
    exit(1);
  }
  fprintf(1, "%d: received pong\n", getpid()); 
  close(p[READ]);

  exit(0); 
}
