#include <fcntl.h>
#include <unistd.h>
#include "../fileio/fileio.h"
#include "../string/string.h"

int file_open_read(const char *path)
{
    return open(path, O_RDONLY);
}

int file_open_write(const char *path)
{
    /* Create file if it doesn't exist, truncate if it does. Permission 0644 */
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}

int file_read_bytes(int fd, void *buf, int count)
{
    if (fd < 0 || buf == (void *)0 || count <= 0)
        return -1;
    return (int)read(fd, buf, count);
}

int file_write_bytes(int fd, const void *buf, int count)
{
    if (fd < 0 || buf == (void *)0 || count <= 0)
        return -1;
    return (int)write(fd, buf, count);
}

void file_close(int fd)
{
    if (fd >= 0)
        close(fd);
}

int file_exists(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd >= 0)
    {
        close(fd);
        return 1;
    }
    return 0;
}

/* Save an integer as text to a file */
int file_save_int(const char *path, int value)
{
    int fd;
    char buf[16];
    int len;

    int_to_string(value, buf, 16);
    len = str_length(buf);

    fd = file_open_write(path);
    if (fd < 0)
        return 0;

    file_write_bytes(fd, buf, len);
    file_close(fd);
    return 1;
}

/* Load an integer from a text file */
int file_load_int(const char *path, int *value)
{
    int fd;
    char buf[16];
    int bytes;

    fd = file_open_read(path);
    if (fd < 0)
        return 0;

    bytes = file_read_bytes(fd, buf, 15);
    file_close(fd);

    if (bytes <= 0)
        return 0;

    buf[bytes] = '\0';
    *value = string_to_int(buf);
    return 1;
}
