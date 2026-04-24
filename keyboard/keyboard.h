#ifndef CUSTOM_KEYBOARD_H
#define CUSTOM_KEYBOARD_H

#define KEY_NONE    0
#define KEY_UP      1000
#define KEY_DOWN    1001
#define KEY_LEFT    1002
#define KEY_RIGHT   1003
#define KEY_ESCAPE  27
#define KEY_ENTER   10

void keyboard_init(void);
void keyboard_restore(void);
int keyboard_pressed(void);
void keyboard_read_line(char *buffer, int max_len);

#endif
