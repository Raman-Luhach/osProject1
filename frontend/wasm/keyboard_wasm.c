/* WASM stub for keyboard — input handled by JS event listeners */
#include "../../keyboard/keyboard.h"

void keyboard_init(void) {}
void keyboard_restore(void) {}
int keyboard_pressed(void) { return KEY_NONE; }
void keyboard_read_line(char *buffer, int max_len) { (void)buffer; (void)max_len; }
