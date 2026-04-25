#include <stdio.h>
#include "../screen/screen.h"

void screen_clear(void)
{
    printf("\033[2J\033[H");
}

void screen_move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

void screen_hide_cursor(void)
{
    printf("\033[?25l");
}

void screen_show_cursor(void)
{
    printf("\033[?25h");
}

void screen_draw_char(int x, int y, char c)
{
    screen_move_cursor(x, y);
    printf("%c", c);
}

void screen_draw_char_color(int x, int y, char c, const char *color)
{
    screen_move_cursor(x, y);
    printf("%s%c%s", color, c, COLOR_RESET);
}

void screen_draw_string(int x, int y, const char *str)
{
    if (str == (void *)0)
        return;
    screen_move_cursor(x, y);
    printf("%s", str);
}

void screen_draw_string_color(int x, int y, const char *str, const char *color)
{
    if (str == (void *)0)
        return;
    screen_move_cursor(x, y);
    printf("%s%s%s", color, str, COLOR_RESET);
}

void screen_draw_box(int x, int y, int width, int height)
{
    int i;

    screen_draw_char(x, y, '+');
    for (i = 1; i < width - 1; i++)
        screen_draw_char(x + i, y, '-');
    screen_draw_char(x + width - 1, y, '+');

    for (i = 1; i < height - 1; i++)
    {
        screen_draw_char(x, y + i, '|');
        screen_draw_char(x + width - 1, y + i, '|');
    }

    screen_draw_char(x, y + height - 1, '+');
    for (i = 1; i < width - 1; i++)
        screen_draw_char(x + i, y + height - 1, '-');
    screen_draw_char(x + width - 1, y + height - 1, '+');
}

void screen_draw_box_color(int x, int y, int width, int height, const char *color)
{
    int i;

    screen_draw_char_color(x, y, '+', color);
    for (i = 1; i < width - 1; i++)
        screen_draw_char_color(x + i, y, '-', color);
    screen_draw_char_color(x + width - 1, y, '+', color);

    for (i = 1; i < height - 1; i++)
    {
        screen_draw_char_color(x, y + i, '|', color);
        screen_draw_char_color(x + width - 1, y + i, '|', color);
    }

    screen_draw_char_color(x, y + height - 1, '+', color);
    for (i = 1; i < width - 1; i++)
        screen_draw_char_color(x + i, y + height - 1, '-', color);
    screen_draw_char_color(x + width - 1, y + height - 1, '+', color);
}

void screen_draw_hline(int x, int y, int length, char c)
{
    int i;

    for (i = 0; i < length; i++)
        screen_draw_char(x + i, y, c);
}

void screen_draw_vline(int x, int y, int length, char c)
{
    int i;

    for (i = 0; i < length; i++)
        screen_draw_char(x, y + i, c);
}

void screen_flush(void)
{
    fflush(stdout);
}
