lockwatch
=========
This is a small set of tools to view and monitor fcntl() style locks on files. It was specifically developed to track locks for exim's retry database, since many versions (including the one we used) are vulnerable to signal race conditions which can leave one process hung locking this file.

In this specific file, the file to check is `/var/spool/exim4/db/retry.lockfile`.

lockinfo
--------
Usage:

    lockinfo <filename>

Runs in a loop and once every seconds outputs if some process has locked the file, and in that case what the pid is of that process.

check_lock_holder
-----------------
This is the nagios plugin version of the tool.

Usage:

    check_lock_holder <filename>

The nagios plugin will run for up to 8 seconds, so don't worry that it takes some time. If a file has been locked *by the same pid* for more than 3 seconds, it sets *WARNING* state. If it has been locked *by the same pid* for more than 8 seconds, it sets *CRITICAL* state.

The times are hardcoded in the C file - the tool isn't really made to be flexible at this point :)
