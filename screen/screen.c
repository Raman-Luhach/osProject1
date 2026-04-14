/* ========================================================================== */
/*                         CUSTOM SCREEN LIBRARY                             */
/*                                                                           */
/*  Implementation of terminal rendering using ANSI escape codes.            */
/*  All output is produced via printf() from <stdio.h>.                     */
/* ========================================================================== */

#include <stdio.h>
#include "../screen/screen.h"
#include "../string/string.h"

/* ========================================================================== */
/*                           SCREEN CONTROL                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  screen_clear                                                             */
/*  Sends the ANSI "erase entire screen" code and moves cursor to (1,1).    */
/* -------------------------------------------------------------------------- */
void screen_clear(void)
{
    printf("\033[2J\033[H");
}

/* -------------------------------------------------------------------------- */
/*  screen_move_cursor                                                       */
/*  Moves the cursor to column x, row y.  ANSI uses row;column order.      */
/* -------------------------------------------------------------------------- */
void screen_move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

/* -------------------------------------------------------------------------- */
/*  screen_hide_cursor / screen_show_cursor                                  */
/*  Toggles cursor visibility for a cleaner game display.                    */
/* -------------------------------------------------------------------------- */
void screen_hide_cursor(void)
{
    printf("\033[?25l");
}

void screen_show_cursor(void)
{
    printf("\033[?25h");
}

/* ========================================================================== */
/*                          DRAWING PRIMITIVES                               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  screen_draw_char                                                         */
/*  Positions the cursor at (x, y) and prints a single character.           */
/* -------------------------------------------------------------------------- */
void screen_draw_char(int x, int y, char c)
{
    screen_move_cursor(x, y);
    printf("%c", c);
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_char_color                                                   */
/*  Same as screen_draw_char but wraps the character in color codes.        */
/* -------------------------------------------------------------------------- */
void screen_draw_char_color(int x, int y, char c, const char *color)
{
    screen_move_cursor(x, y);
    printf("%s%c%s", color, c, COLOR_RESET);
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_string                                                       */
/*  Prints a full string starting at position (x, y).                       */
/* -------------------------------------------------------------------------- */
void screen_draw_string(int x, int y, const char *str)
{
    if (str == (void *)0)
        return;
    screen_move_cursor(x, y);
    printf("%s", str);
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_string_color                                                 */
/*  Prints a colored string at position (x, y).                             */
/* -------------------------------------------------------------------------- */
void screen_draw_string_color(int x, int y, const char *str,
                               const char *color)
{
    if (str == (void *)0)
        return;
    screen_move_cursor(x, y);
    printf("%s%s%s", color, str, COLOR_RESET);
}

/* ========================================================================== */
/*                            SHAPE DRAWING                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  screen_draw_box                                                          */
/*  Draws a box outline. The top-left corner is at (x, y).                  */
/*  Width and height include the border characters themselves.               */
/*                                                                           */
/*  Example 5x3 box:                                                        */
/*    +---+                                                                  */
/*    |   |                                                                  */
/*    +---+                                                                  */
/* -------------------------------------------------------------------------- */
void screen_draw_box(int x, int y, int width, int height)
{
    int i;

    /* Top border: +---+ */
    screen_draw_char(x, y, '+');
    for (i = 1; i < width - 1; i++)
        screen_draw_char(x + i, y, '-');
    screen_draw_char(x + width - 1, y, '+');

    /* Side borders: |   | */
    for (i = 1; i < height - 1; i++)
    {
        screen_draw_char(x, y + i, '|');
        screen_draw_char(x + width - 1, y + i, '|');
    }

    /* Bottom border: +---+ */
    screen_draw_char(x, y + height - 1, '+');
    for (i = 1; i < width - 1; i++)
        screen_draw_char(x + i, y + height - 1, '-');
    screen_draw_char(x + width - 1, y + height - 1, '+');
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_box_color                                                    */
/*  Same as screen_draw_box but every border character uses the given color. */
/* -------------------------------------------------------------------------- */
void screen_draw_box_color(int x, int y, int width, int height,
                            const char *color)
{
    int i;

    /* Top border */
    screen_draw_char_color(x, y, '+', color);
    for (i = 1; i < width - 1; i++)
        screen_draw_char_color(x + i, y, '-', color);
    screen_draw_char_color(x + width - 1, y, '+', color);

    /* Side borders */
    for (i = 1; i < height - 1; i++)
    {
        screen_draw_char_color(x, y + i, '|', color);
        screen_draw_char_color(x + width - 1, y + i, '|', color);
    }

    /* Bottom border */
    screen_draw_char_color(x, y + height - 1, '+', color);
    for (i = 1; i < width - 1; i++)
        screen_draw_char_color(x + i, y + height - 1, '-', color);
    screen_draw_char_color(x + width - 1, y + height - 1, '+', color);
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_hline                                                        */
/*  Draws a horizontal line of repeated characters starting at (x, y).      */
/* -------------------------------------------------------------------------- */
void screen_draw_hline(int x, int y, int length, char c)
{
    int i;

    for (i = 0; i < length; i++)
        screen_draw_char(x + i, y, c);
}

/* -------------------------------------------------------------------------- */
/*  screen_draw_vline                                                        */
/*  Draws a vertical line of repeated characters starting at (x, y).        */
/* -------------------------------------------------------------------------- */
void screen_draw_vline(int x, int y, int length, char c)
{
    int i;

    for (i = 0; i < length; i++)
        screen_draw_char(x, y + i, c);
}

/* ========================================================================== */
/*                           BUFFER CONTROL                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  screen_flush                                                             */
/*  Forces all buffered output to appear on screen immediately.              */
/*  Call this once at the end of each frame for flicker-free rendering.      */
/* -------------------------------------------------------------------------- */
void screen_flush(void)
{
    fflush(stdout);
}
