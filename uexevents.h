#include <stdio.h>
#include <stdint.h>
#include <libinput.h>
#include <libevdev/libevdev.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#define FIFO_PATH "/dev/uexevents"

static int fifo_fd = -1;

#include "fifo.h"
#include "reader.h"