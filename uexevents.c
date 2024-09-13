#include "uexevents.h"

int main() {

	struct sigaction sa;
	sa.sa_handler = handle_sigpipe;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, NULL);

	struct udev *udev = udev_new();
	if (udev == NULL)
	{
		fprintf(stderr, "Failed to initialize udev.\n");
		return 1;
	}

	struct libinput *libinput = libinput_udev_create_context(&interface, NULL, udev);
	if (!libinput)
	{
		fprintf(stderr, "Failed to initialize libinput from udev.\n");
		udev_unref(udev);
		return 1;
	}

	if (libinput_udev_assign_seat(libinput, "seat0") != 0)
	{
		fprintf(stderr, "Failed to set seat.\n");
		libinput_unref(libinput);
		udev_unref(udev);
		return 1;
	}

	// Create the FIFO if it does not already exist
	if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST)
	{
		perror("Failed to create FIFO");
		libinput_unref(libinput);
		udev_unref(udev);
		return 1;
	}

	// Open the FIFO in blocking mode initially
	fifo_fd = open(FIFO_PATH, O_WRONLY);
	if (fifo_fd < 0) {
		perror("Failed to open FIFO");
		libinput_unref(libinput);
		udev_unref(udev);
		return 1;
	}

	// Set the file descriptor to non-blocking mode
	int flags = fcntl(fifo_fd, F_GETFL);
	if (flags < 0)
	{
		perror("Failed to get file descriptor flags");
		libinput_unref(libinput);
		udev_unref(udev);
		return 1;
	}

	flags |= O_NONBLOCK;
	if (fcntl(fifo_fd, F_SETFL, flags) < 0)
	{
		perror("Failed to set file descriptor flags");
		libinput_unref(libinput);
		udev_unref(udev);
		return 1;
	}

	run_mainloop(libinput);

	if (fifo_fd >= 0)
	{
		close(fifo_fd);
	}
	libinput_unref(libinput);
	udev_unref(udev);

	return 0;
}