CFLAGS=-Wall
all: lockinfo check_lock_holder

lockinfo: lockinfo.c

check_lock_holder: check_lock_holder.c

clean:
	rm -f lockinfo check_lock_holder