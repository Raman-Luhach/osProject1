/* WASM stub for screen — rendering handled by JS Canvas */
#include "../../screen/screen.h"

void screen_clear(void) {}
void screen_move_cursor(int x, int y) { (void)x; (void)y; }
void screen_hide_cursor(void) {}
void screen_show_cursor(void) {}
void screen_draw_char(int x, int y, char c) { (void)x; (void)y; (void)c; }
void screen_draw_char_color(int x, int y, char c, const char *color)
    { (void)x; (void)y; (void)c; (void)color; }
void screen_draw_string(int x, int y, const char *str) { (void)x; (void)y; (void)str; }
void screen_draw_string_color(int x, int y, const char *str, const char *color)
    { (void)x; (void)y; (void)str; (void)color; }
void screen_draw_box(int x, int y, int w, int h) { (void)x; (void)y; (void)w; (void)h; }
void screen_draw_box_color(int x, int y, int w, int h, const char *color)
    { (void)x; (void)y; (void)w; (void)h; (void)color; }
void screen_draw_hline(int x, int y, int len, char c) { (void)x; (void)y; (void)len; (void)c; }
void screen_draw_vline(int x, int y, int len, char c) { (void)x; (void)y; (void)len; (void)c; }
void screen_flush(void) {}
