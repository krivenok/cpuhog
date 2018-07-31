# cpuhog
Simple CPU hogging tool for testing purposes

## How to build it?

The build system for cpuhog is a trivial Makefile, so you can build it as follows:
```
dk@fedora 16:46:23 ~/git/cpuhog $ [0] make
gcc -Wall -Wextra -lpthread cpuhog.c -o cpuhog
dk@fedora 16:46:28 ~/git/cpuhog $ [0]
```

## How to run it?

If you run it without arguments, it will create a CPU hogging thread per each available CPU core and will run infinitely.

```
dk@fedora 16:48:03 ~/git/cpuhog $ [0] ./cpuhog
^C
dk@fedora 16:48:14 ~/git/cpuhog $ [130]
```

You can also use environment variables to change cpuhog behavior

* CPUHOG_DEBUG - if set, enables debug logging
* CPUHOG_DURATION - if set, defines the duration of the execution in seconds
* CPUHOG_SYSCALL - if set, forces cpuhog to do a syscall at each iteration of tight loop (gettid is used now)

The example below demonstrates the use of environment variables.

```
dk@fedora 16:50:13 ~/git/cpuhog $ [0] CPUHOG_DURATION=3 CPUHOG_DEBUG=1 ./cpuhog
Detected 2 available CPU cores
CPU hog thread 1 started
CPU hog thread 0 started
Affined CPU hog thread 0 to CPU core
Changed CPU hog thread 0 name to cpuhog00000
Affined CPU hog thread 1 to CPU core
Changed CPU hog thread 1 name to cpuhog00001
Got signal 14, exiting.
dk@fedora 16:50:25 ~/git/cpuhog $ [0]
```

While cpuhog is running, you can run `top -H` to see its threads consuming the CPU.

```
top - 16:58:56 up  1:28,  1 user,  load average: 0.31, 0.13, 0.10
Threads: 148 total,   3 running,  96 sleeping,   0 stopped,   0 zombie
%Cpu(s): 99.3 us,  0.2 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.4 si,  0.0 st
KiB Mem :  4039212 total,  3003676 free,   236504 used,   799032 buff/cache
KiB Swap:  4177916 total,  4177916 free,        0 used.  3511692 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S %CPU %MEM     TIME+ COMMAND
  381 dk        20   0   22972    776    696 R 99.9  0.0   0:08.28 cpuhog00000
  382 dk        20   0   22972    776    696 R 99.9  0.0   0:08.28 cpuhog00001
 1070 dk        20   0   25748   4144   2928 S  0.3  0.1   0:01.70 tmux: server
    1 root      20   0  223740  11528   8156 S  0.0  0.3   0:01.70 systemd
...
...
```
