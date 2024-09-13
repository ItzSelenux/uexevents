# UEXEVENTS

---
Udev Events eXposer, expose udev events into the filesystem throught a fifo device

## Why?

Many programs that read the keys in wayland (or X.org using the udev metod), read directly from udev independently, this is to a certain extent chaotic and is a waste of resources that could be considered unnecessary, uexevents aims to provide a more universal method for this type of programs.

## Usage

Uexevents rather than a library, is a daemon that parse and writes udev events into /dev/uexevents, so if the daemon is running, is possible to access even with shell commands like `cat`

## Features / TO-DO

[x] FIFO Device for universal access
[x] Parse udev events to a more reliable format
[ ] Get Status for keys like Caps/Num lock
[ ] Get key case for letters
[ ] Control to stop/start the daemon without root (useful for security when writing passwords)

### Installation

### Build
```
make uexevents
```