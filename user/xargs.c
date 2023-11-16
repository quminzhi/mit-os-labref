#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUF_SIZE 512

/**
 * A simple implementation of xargs.
 * 
 * ex>
 * $ echo hello too | xargs echo bye
 * bye hello too
 * $ echo hello\ntoo | xargs echo bye
 * bye hello
 * bye too
 * 
 * ref: pipe in sh.c
 */

int main(int argc, char *argv[]) {
  int i, n, pid;
  char *cmd[MAXARG];
  char ch, buf[BUF_SIZE], *p;

  if (argc < 2) {
    fprintf(2, "Usage: xargs <cmd_list>\n");
    exit(1);
  }

  n = 0;
  for (i = 1; i <= argc - 1; i++) {
    cmd[n] = malloc(strlen(argv[i]) + 1);
    strcpy(cmd[n++], argv[i]);
  }

  p = buf;
  // read a line from fd=0 and exec cmd with the line as arguments 
  // BUG: refer to getcmd in sh.c
  while (read(0, &ch, sizeof(ch)) == 1) {
    if (ch == '\n') {
      *p = 0;
      pid = fork();
      if (pid == 0) {
        // exec
        if (cmd[n] != 0) {
          free(cmd[n]);
        }
        cmd[n] = malloc(strlen(buf) + 1);
        strcpy(cmd[n++], buf);
        // null terminator
        cmd[n++] = 0;
        exec(cmd[0], cmd);
        fprintf(2, "xargs: exec error.\n");
        exit(1);
      }
      wait((int *)0);
      // reset p
      p = buf;
    } else {
      *p++ = ch;
      if (p >= buf + BUF_SIZE) {
        fprintf(2, "xargs: input argument is too long.\n");
        exit(1);
      }
    }
  }

  for (int i = 0; i < n; i++) {
    if (cmd[i] != 0) {
      free(cmd[i]);
    }
  }

  exit(0);
}
