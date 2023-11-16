#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

/**
 * An implementation of a concurrent version of prime sieve using pipes.
 * 
 * ref: https://swtch.com/~rsc/thread
 */

void sieve(int r, int w) {
  int pid, p, n, c;
  int pn[2];
  close(w);

  c = read(r, &p, sizeof(int));
  if (c == 0) {
    close(r);
    return;
  }
  if (c != 4) {
    fprintf(2, "Error: read in sieve process.\n");
    close(r);
    exit(-1);
  }

  printf("prime %d\n", p);

  // create pipe and fork a child process
  pipe(pn);
  pid = fork();
  if (pid == 0) {
    sieve(pn[0], pn[1]);
    exit(0);
  }

  // filter by divisibility
  close(pn[READ]);
  while (read(r, &n, sizeof(int)) == 4) {
    if (n % p != 0) {
      write(pn[WRITE], &n, sizeof(n));
    }
  }

  close(pn[WRITE]);
  close(r);

  // wait for the child process to exit
  wait((int *)0);
}

int main(int argc, char *argv[]) {
  int pid;
  int p[2];

  pipe(p);

  pid = fork();
  if (pid == 0) {
    sieve(p[READ], p[WRITE]);
    exit(0);
  }

  // generate numbers from 2 to 35
  close(p[READ]);
  for (int i = 2; i <= 35; i++) {
    write(p[WRITE], &i, sizeof(int));
  }
  close(p[WRITE]);

  // reap any exited child process
  wait((int *)0);

  exit(0);
}
