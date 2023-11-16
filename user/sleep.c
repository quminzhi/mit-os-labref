#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/**
 * A simple implementation of sleep
 */

int main(int argc, char *argv[]) {
  int ts;

  if (argc != 2) {
    fprintf(2, "Usage: sleep ts...\n");
    exit(1);
  }

  if ((ts = atoi(argv[1])) == -1) {
    fprintf(2, "Error occurs when calling atoi.\n");
    exit(1);
  }

  if (sleep(ts) == -1) {
    fprintf(2, "Error in syscall to sleep.\n");
    exit(1);
  } 

  exit(0);
}
