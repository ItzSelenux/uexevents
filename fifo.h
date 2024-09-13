
void handle_sigpipe() {
    fprintf(stderr, "Received SIGPIPE signal. Reinitializing FIFO...\n");
    if (fifo_fd >= 0) {
        close(fifo_fd);
        fifo_fd = -1;
    }

    // Reopen the FIFO in non-blocking mode
    fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd < 0) {
        perror("Failed to reopen FIFO");
        return;
    }

    // Set the file descriptor to non-blocking mode
    int flags = fcntl(fifo_fd, F_GETFL);
    if (flags < 0) {
        perror("Failed to get file descriptor flags");
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fifo_fd, F_SETFL, flags) < 0) {
        perror("Failed to set file descriptor flags");
        return;
    }
}