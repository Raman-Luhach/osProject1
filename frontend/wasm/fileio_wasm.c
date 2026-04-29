/* WASM stub for fileio — high score handled by JS localStorage */
#include "../../fileio/fileio.h"
#include "../../string/string.h"

int file_open_read(const char *path) { (void)path; return -1; }
int file_open_write(const char *path) { (void)path; return -1; }
int file_read_bytes(int fd, void *buf, int count) { (void)fd; (void)buf; (void)count; return -1; }
int file_write_bytes(int fd, const void *buf, int count) { (void)fd; (void)buf; (void)count; return -1; }
void file_close(int fd) { (void)fd; }
int file_exists(const char *path) { (void)path; return 0; }
int file_save_int(const char *path, int value) { (void)path; (void)value; return 0; }
int file_load_int(const char *path, int *value) { (void)path; (void)value; return 0; }
