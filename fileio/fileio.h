#ifndef CUSTOM_FILEIO_H
#define CUSTOM_FILEIO_H

/* Opens a file, returns file descriptor (-1 on error) */
int file_open_read(const char *path);
int file_open_write(const char *path);

/* Read/write raw bytes, returns bytes read/written */
int file_read_bytes(int fd, void *buf, int count);
int file_write_bytes(int fd, const void *buf, int count);

/* Close a file descriptor */
void file_close(int fd);

/* Returns 1 if file exists, 0 otherwise */
int file_exists(const char *path);

/* High-level: save and load a single integer (for high scores) */
int file_save_int(const char *path, int value);
int file_load_int(const char *path, int *value);

#endif
