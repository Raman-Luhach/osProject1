/* ========================================================================== */
/*                        CUSTOM KEYBOARD LIBRARY                            */
/*                                                                           */
/*  Provides non-blocking and blocking keyboard input functions.             */
/*  Uses POSIX <termios.h> to configure the terminal for raw input mode.    */
/*                                                                           */
/*  Usage:                                                                   */
/*    1. Call keyboard_init() once at program start.                        */
/*    2. Use keyboard_pressed() in the game loop (non-blocking).            */
/*    3. Use keyboard_read_line() for menus (blocking, with echo).          */
/*    4. Call keyboard_restore() before exiting (or register with atexit).  */
/* ========================================================================== */

#ifndef CUSTOM_KEYBOARD_H
#define CUSTOM_KEYBOARD_H

/* ---- Special Key Codes ---- */
/* Regular ASCII keys (a-z, 0-9, etc.) are returned as their char value.    */
/* Arrow keys and special keys use these codes (above ASCII range).         */

#define KEY_NONE    0       /* No key was pressed (non-blocking returned)    */
#define KEY_UP      1000    /* Up arrow    or 'w'                           */
#define KEY_DOWN    1001    /* Down arrow  or 's'                           */
#define KEY_LEFT    1002    /* Left arrow  or 'a'                           */
#define KEY_RIGHT   1003    /* Right arrow or 'd'                           */
#define KEY_ESCAPE  27      /* Escape key                                    */
#define KEY_ENTER   10      /* Enter / Return key                           */

/* ---- Functions ---- */

/* Switches the terminal to raw mode (no echo, no line buffering).          */
/* Must be called before keyboard_pressed() will work.                       */
void keyboard_init(void);

/* Restores the terminal to its original settings.                           */
/* Should be called before the program exits.                                */
/* Safe to call multiple times.                                              */
void keyboard_restore(void);

/* Non-blocking key read.                                                    */
/* Returns the key code if a key is available, or KEY_NONE (0) if not.      */
/* Arrow keys are returned as KEY_UP / KEY_DOWN / KEY_LEFT / KEY_RIGHT.     */
int keyboard_pressed(void);

/* Blocking line read (for menus and prompts).                               */
/* Temporarily restores echo and line-buffered mode.                         */
/* Reads up to (max_len - 1) characters into buffer, null-terminates.       */
void keyboard_read_line(char *buffer, int max_len);

#endif /* CUSTOM_KEYBOARD_H */
