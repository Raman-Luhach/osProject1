#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

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

#define BG_RED               "\033[41m"
#define BG_GREEN             "\033[42m"
#define BG_BLUE              "\033[44m"
#define BG_CYAN              "\033[46m"
#define BG_WHITE             "\033[47m"

void screen_clear(void);
void screen_move_cursor(int x, int y);
void screen_hide_cursor(void);
void screen_show_cursor(void);
void screen_draw_char(int x, int y, char c);
void screen_draw_char_color(int x, int y, char c, const char *color);
void screen_draw_string(int x, int y, const char *str);
void screen_draw_string_color(int x, int y, const char *str, const char *color);
void screen_draw_box(int x, int y, int width, int height);
void screen_draw_box_color(int x, int y, int width, int height, const char *color);
void screen_draw_hline(int x, int y, int length, char c);
void screen_draw_vline(int x, int y, int length, char c);
void screen_flush(void);

#endif
