// Restricted file access functions for libinput
static int open_restricted(const char *path, int flags, void *user_data)
{
	int fd = open(path, flags);
	if (fd < 0)
	{
		perror("Failed to open restricted path");
	}
	return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
	close(fd);
}

static const struct libinput_interface interface =
{
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
};

// Function to write event data to the FIFO
static void write_to_fifo(const char *data)
{
	if (fifo_fd < 0)
	{
		fprintf(stderr, "FIFO is not open\n");
		return;
	}

	ssize_t bytes_written = write(fifo_fd, data, strlen(data));
	if (bytes_written < 0)
	{
		if (errno == EPIPE)
		{
			fprintf(stderr, "Broken pipe detected, reopening FIFO...\n");
			handle_sigpipe(SIGPIPE);
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			// Handle non-blocking I/O error
			fprintf(stderr, "Non-blocking I/O error, retrying...\n");
		}
		else
		{
			perror("Failed to write to FIFO");
		}
	}
}

// Event printing functions
static int print_key_event(struct libinput_event *event)
{
	struct libinput_event_keyboard *keyboard = libinput_event_get_keyboard_event(event);

	uint32_t time_stamp = libinput_event_keyboard_get_time(keyboard);
	uint32_t key_code = libinput_event_keyboard_get_key(keyboard);
	const char *key_name = libevdev_event_code_get_name(EV_KEY, key_code);
	key_name = key_name ? key_name : "null";
	enum libinput_key_state state_code = libinput_event_keyboard_get_key_state(keyboard);
	const char *state_name = state_code == LIBINPUT_KEY_STATE_PRESSED ? "PRESSED" : "RELEASED";

	char buffer[256];
	snprintf(buffer, sizeof(buffer), "{"
				"\"event_name\": \"KEYBOARD_KEY\", "
				"\"time_stamp\": %d, "
				"\"key_name\": \"%s\", "
				"\"key_code\": %d, "
				"\"state_name\": \"%s\", "
				"\"state_code\": %d"
				"}\n",
				time_stamp, key_name, key_code, state_name, state_code);

	write_to_fifo(buffer);
	return 0;
}

static int print_mouse_button_event(struct libinput_event *event)
{
	struct libinput_event_pointer *pointer = libinput_event_get_pointer_event(event);

	uint32_t time_stamp = libinput_event_pointer_get_time(pointer);
	uint32_t button_code = libinput_event_pointer_get_button(pointer);
	const char *button_name = libevdev_event_code_get_name(EV_KEY, button_code);
	button_name = button_name ? button_name : "null";
	enum libinput_button_state state_code = libinput_event_pointer_get_button_state(pointer);
	const char *state_name = state_code == LIBINPUT_BUTTON_STATE_PRESSED ? "PRESSED" : "RELEASED";

	char buffer[256];
	snprintf(buffer, sizeof(buffer), "{"
				"\"event_name\": \"MOUSE_BUTTON\", "
				"\"time_stamp\": %d, "
				"\"button_name\": \"%s\", "
				"\"button_code\": %d, "
				"\"state_name\": \"%s\", "
				"\"state_code\": %d"
				"}\n",
				time_stamp, button_name, button_code, state_name, state_code);

	write_to_fifo(buffer);
	return 0;
}

static int print_mouse_motion_event(struct libinput_event *event)
{
	struct libinput_event_pointer *pointer = libinput_event_get_pointer_event(event);

	uint32_t time_stamp = libinput_event_pointer_get_time(pointer);
	double dx = libinput_event_pointer_get_dx(pointer);
	double dy = libinput_event_pointer_get_dy(pointer);

	char buffer[256];
	snprintf(buffer, sizeof(buffer), "{"
				"\"event_name\": \"MOUSE_MOTION\", "
				"\"time_stamp\": %d, "
				"\"dx\": %f, "
				"\"dy\": %f"
				"}\n",
				time_stamp, dx, dy);

	write_to_fifo(buffer);
	return 0;
}

static int handle_events(struct libinput *libinput)
{
	struct libinput_event *event;

	if (libinput_dispatch(libinput) < 0)
		return -1;

	while ((event = libinput_get_event(libinput)) != NULL)
	{
		switch (libinput_event_get_type(event))
		{
			case LIBINPUT_EVENT_KEYBOARD_KEY:
				print_key_event(event);
				break;
			case LIBINPUT_EVENT_POINTER_BUTTON:
				print_mouse_button_event(event);
				break;
			case LIBINPUT_EVENT_POINTER_MOTION:
				print_mouse_motion_event(event);
				break;
			default:
				break;
		}
		libinput_event_destroy(event);
	}
	return 0;
}

static int run_mainloop(struct libinput *libinput)
{
	struct pollfd fd;
	fd.fd = libinput_get_fd(libinput);
	fd.events = POLLIN;

	while (poll(&fd, 1, -1) > -1)
		handle_events(libinput);

	return 0;
}