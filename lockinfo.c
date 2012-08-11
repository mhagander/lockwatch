#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filaname>\n", argv[0]);
    exit(1);
  }
  
  while (1) {
    int fd = open(argv[1], O_RDONLY);
    struct flock lock;
    if (fd < 0) {
      fprintf(stderr, "Could not open %s: %m\n", argv[1]);
      exit(1);
    }
    memset(&lock, sizeof(lock), 0);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_GETLK, &lock) != 0) {
      fprintf(stderr, "Failed to fcntl: %m\n");
      exit(1);
    }
    if (lock.l_type == F_UNLCK) {
      printf("Not locked\n");
    }
    else {
      printf("Locked by pid %i\n", lock.l_pid);
    }
    close(fd);
    fflush(stdout);
    sleep(1);
  }
}

