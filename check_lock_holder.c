#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/*
 * Nagios plugin that checks for fcntl() locks.
 *
 * Verifies if the same process has a lock on a file for longer
 * than a certain time.
 *
 * The timeouts are currently hardcoded at 3 and 7 seconds.
 *
 * The plugin will run for as much as CRITICAL_SECONDS seconds,
 * so don't set it stupidly high.
 */

#define WARNING_SECONDS 3
#define CRITICAL_SECONDS 7

int main(int argc, char *argv[])
{
  int runseconds = 0;
  pid_t locked_by_pid = -1;
  int fd = -1;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(3);
  }
  
  while (runseconds < CRITICAL_SECONDS) {
    fd = open(argv[1], O_RDONLY);
    struct flock lock;
    if (fd < 0) {
      fprintf(stderr, "Could not open %s: %m\n", argv[1]);
      exit(3);
    }
    memset(&lock, sizeof(lock), 0);
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_GETLK, &lock) != 0) {
      fprintf(stderr, "Failed to fcntl: %m\n");
      exit(3);
    }
    if (lock.l_type == F_UNLCK) {
      /* Currently not locked, so we're done */
      break;
    }
    else {
      /* Locked by someone. */
      if (locked_by_pid == -1) {
	/*
	 * Not locked by anybody before, so this is the first run.
	 * Record who has the lock so we can check it later.
	 */
	locked_by_pid = lock.l_pid;
      }
      else if (locked_by_pid != lock.l_pid) {
	/*
	 * The owner of the lock has changed. That means that the
	 * lock was dropped.
	 */
	break;
      }
      /* Else the lock is still being held, so we loop */
    }
    close(fd);
    fd = -1;

    runseconds++;
    sleep(1);
  }
  if (fd != -1)
    close(fd);

  /* Now check the time */
  if (runseconds < WARNING_SECONDS) {
    printf("OK: %s not locked more than %d seconds\n", argv[1], WARNING_SECONDS);
    exit(0);
  }
  if (runseconds < CRITICAL_SECONDS) {
    printf("WARNING: %s locked by pid %d for %d seconds\n", argv[1], locked_by_pid, runseconds);
    exit(1);
  }
  printf("CRITICAL: %s locked by pid %d for more than %d seconds\n", argv[1], locked_by_pid, runseconds);
  exit(2);
}

