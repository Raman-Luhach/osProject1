#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "../keyboard/keyboard.h"

static struct termios original_termios;
static struct termios raw_termios;
static int is_raw_mode = 0;

void keyboard_init(void)
{
    if (is_raw_mode)
        return;

    tcgetattr(STDIN_FILENO, &original_termios);
    raw_termios = original_termios;
    raw_termios.c_lflag &= (unsigned long)~(ICANON | ECHO);
    raw_termios.c_cc[VMIN] = 0;
    raw_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
    is_raw_mode = 1;
}

void keyboard_restore(void)
{
    if (!is_raw_mode)
        return;
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    is_raw_mode = 0;
}

int keyboard_pressed(void)
{
    unsigned char buf[3];
    int bytes_read;

    bytes_read = (int)read(STDIN_FILENO, buf, 3);
    if (bytes_read <= 0)
        return KEY_NONE;

    if (bytes_read == 1)
        return (int)buf[0];

    if (bytes_read >= 3 && buf[0] == 27 && buf[1] == '[')
    {
        switch (buf[2])
        {
            case 'A': return KEY_UP;
            case 'B': return KEY_DOWN;
            case 'C': return KEY_RIGHT;
            case 'D': return KEY_LEFT;
            default:  break;
        }
    }

    if (bytes_read == 2 && buf[0] == 27)
        return KEY_ESCAPE;

    return (int)buf[0];
}

void keyboard_read_line(char *buffer, int max_len)
{
    int i;

    if (buffer == (void *)0 || max_len <= 0)
        return;

    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);

    if (fgets(buffer, max_len, stdin) != (void *)0)
    {
        i = 0;
        while (buffer[i] != '\0')
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
            {
                buffer[i] = '\0';
                break;
            }
            i++;
        }
    }
    else
        buffer[0] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
}
