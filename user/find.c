#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAX_PATH_SIZE 512

/**
 * A simple implementation of UNIX find.
 */

char *basename(char *path) {
  char *p;

  // find the last '/'
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(char *path, char *pattern) {
  char buf[MAX_PATH_SIZE], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if (strcmp(pattern, basename(path)) == 0) {
      fprintf(1, "%s\n", path);
    }
    break;
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    // move to the end of string
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }
      // copy until '\0' is hitted
      memmove(p, de.name, DIRSIZ);
      // for the extreme case when length of dirname is DIRSIZ
      p[DIRSIZ] = 0;
      find(buf, pattern);
    }

    break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <pattern>\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}
