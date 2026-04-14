/* ========================================================================== */
/*                         CUSTOM SCREEN LIBRARY                             */
/*                                                                           */
/*  Provides terminal rendering functions using ANSI escape codes.           */
/*  All output goes through printf() from <stdio.h> (the only allowed I/O). */
/*                                                                           */
/*  Coordinate system:                                                       */
/*    x = column (1 = leftmost)                                             */
/*    y = row    (1 = topmost)                                              */
/* ========================================================================== */

#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

/* ---- ANSI Color Codes ---- */
/* Use these constants with the _color drawing functions. */

#define COLOR_RESET          "\033[0m"
#define COLOR_BOLD           "\033[1m"
#define COLOR_DIM            "\033[2m"

#define COLOR_RED            "\033[31m"
#define COLOR_GREEN          "\033[32m"
#define COLOR_YELLOW         "\033[33m"
#define COLOR_BLUE           "\033[34m"
#define COLOR_MAGENTA        "\033[35m"
#define COLOR_CYAN           "\033[36m"
#define COLOR_WHITE          "\033[37m"

#define COLOR_BRIGHT_RED     "\033[91m"
#define COLOR_BRIGHT_GREEN   "\033[92m"
#define COLOR_BRIGHT_YELLOW  "\033[93m"
#define COLOR_BRIGHT_BLUE    "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN    "\033[96m"
#define COLOR_BRIGHT_WHITE   "\033[97m"

/* Background colors */
#define BG_RED               "\033[41m"
#define BG_GREEN             "\033[42m"
#define BG_BLUE              "\033[44m"
#define BG_CYAN              "\033[46m"
#define BG_WHITE             "\033[47m"

/* ---- Screen Control ---- */

/* Clears the entire terminal and moves cursor to top-left corner. */
void screen_clear(void);

/* Moves the cursor to column x, row y (1-based). */
void screen_move_cursor(int x, int y);

/* Hides the blinking text cursor (cleaner look during gameplay). */
void screen_hide_cursor(void);

/* Restores the blinking text cursor. */
void screen_show_cursor(void);

/* ---- Drawing Primitives ---- */

/* Draws a single character at position (x, y). */
void screen_draw_char(int x, int y, char c);

/* Draws a single character at position (x, y) with the given ANSI color. */
void screen_draw_char_color(int x, int y, char c, const char *color);

/* Draws a null-terminated string starting at position (x, y). */
void screen_draw_string(int x, int y, const char *str);

/* Draws a null-terminated string at (x, y) with the given ANSI color. */
void screen_draw_string_color(int x, int y, const char *str,
                               const char *color);

/* ---- Shape Drawing ---- */

/* Draws a rectangular box outline using +, -, | characters.                */
/* (x, y) is the top-left corner. Width and height include the border.      */
void screen_draw_box(int x, int y, int width, int height);

/* Same as screen_draw_box but with the given ANSI color. */
void screen_draw_box_color(int x, int y, int width, int height,
                            const char *color);

/* Draws a horizontal line of 'length' characters starting at (x, y). */
void screen_draw_hline(int x, int y, int length, char c);

/* Draws a vertical line of 'length' characters starting at (x, y). */
void screen_draw_vline(int x, int y, int length, char c);

/* ---- Buffer Control ---- */

/* Flushes stdout so all pending characters appear on screen immediately.   */
/* Call this once per frame after all drawing is done.                        */
void screen_flush(void);

#endif /* CUSTOM_SCREEN_H */
