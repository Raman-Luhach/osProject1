/* ========================================================================== */
/*                        CUSTOM KEYBOARD LIBRARY                            */
/*                                                                           */
/*  Implementation of non-blocking and blocking keyboard input.              */
/*                                                                           */
/*  Terminal Modes:                                                          */
/*    - Canonical mode (default): input is line-buffered, echo is on.       */
/*    - Raw mode (our mode): every keypress is available immediately,       */
/*      no echo, no special character processing.                            */
/*                                                                           */
/*  Arrow Key Encoding (ANSI terminals):                                    */
/*    Arrow keys send 3-byte escape sequences:                              */
/*      Up    = ESC [ A  (27, 91, 65)                                       */
/*      Down  = ESC [ B  (27, 91, 66)                                       */
/*      Right = ESC [ C  (27, 91, 67)                                       */
/*      Left  = ESC [ D  (27, 91, 68)                                       */
/* ========================================================================== */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "../keyboard/keyboard.h"

/* ---- Module State ---- */

/* Saved copy of the terminal's original settings (restored on exit). */
static struct termios original_termios;

/* Our modified raw-mode settings. */
static struct termios raw_termios;

/* Flag to prevent double-init or double-restore. */
static int is_raw_mode = 0;

/* ========================================================================== */
/*                            INITIALIZATION                                 */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  keyboard_init                                                            */
/*  Saves the current terminal settings, then switches to raw mode:         */
/*    - ICANON off: don't wait for Enter before returning input.            */
/*    - ECHO off:   don't print typed characters to the screen.             */
/*    - VMIN = 0:   read() returns immediately even with no data.           */
/*    - VTIME = 0:  no timeout on read().                                    */
/* -------------------------------------------------------------------------- */
void keyboard_init(void)
{
    if (is_raw_mode)
        return;  /* Already initialized */

    /* Save original settings so we can restore them later */
    tcgetattr(STDIN_FILENO, &original_termios);

    /* Copy the original and modify for raw mode */
    raw_termios = original_termios;
    raw_termios.c_lflag &= (unsigned long)~(ICANON | ECHO);
    raw_termios.c_cc[VMIN]  = 0;   /* Non-blocking: return immediately     */
    raw_termios.c_cc[VTIME] = 0;   /* No timeout                           */

    /* Apply the raw settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);

    is_raw_mode = 1;
}

/* -------------------------------------------------------------------------- */
/*  keyboard_restore                                                         */
/*  Restores the terminal to its original settings.                          */
/*  Safe to call multiple times (only acts once).                            */
/* -------------------------------------------------------------------------- */
void keyboard_restore(void)
{
    if (!is_raw_mode)
        return;

    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    is_raw_mode = 0;
}

/* ========================================================================== */
/*                            INPUT READING                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  keyboard_pressed                                                         */
/*  Non-blocking read: checks if a key has been pressed.                     */
/*                                                                           */
/*  Returns:                                                                 */
/*    - KEY_NONE (0)  if no key is available.                               */
/*    - KEY_UP/DOWN/LEFT/RIGHT for arrow keys.                              */
/*    - The ASCII value for regular keys (e.g., 'q', ' ', etc.).            */
/*                                                                           */
/*  Arrow Key Detection:                                                     */
/*    When an arrow key is pressed, the terminal sends 3 bytes:             */
/*      byte[0] = 27 (ESC), byte[1] = 91 ('['), byte[2] = 65..68           */
/*    We read all 3 and decode the direction.                                */
/* -------------------------------------------------------------------------- */
int keyboard_pressed(void)
{
    unsigned char buf[3];
    int           bytes_read;

    /* Attempt to read up to 3 bytes (enough for an arrow key sequence) */
    bytes_read = (int)read(STDIN_FILENO, buf, 3);

    /* No key available */
    if (bytes_read <= 0)
        return KEY_NONE;

    /* ---- Single-byte key ---- */
    if (bytes_read == 1)
        return (int)buf[0];

    /* ---- Multi-byte escape sequence (arrow keys) ---- */
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

    /* ---- Two-byte: bare ESC + something else ---- */
    if (bytes_read == 2 && buf[0] == 27)
        return KEY_ESCAPE;

    /* Return the first byte for any unrecognized sequence */
    return (int)buf[0];
}

/* -------------------------------------------------------------------------- */
/*  keyboard_read_line                                                       */
/*  Blocking read for complete lines (used in menus/prompts).                */
/*                                                                           */
/*  Temporarily restores canonical mode and echo so the user can see what   */
/*  they type, use backspace, etc.  After reading, switches back to raw.    */
/* -------------------------------------------------------------------------- */
void keyboard_read_line(char *buffer, int max_len)
{
    int i;

    if (buffer == (void *)0 || max_len <= 0)
        return;

    /* Temporarily switch to canonical mode with echo */
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);

    /* Read a full line using stdio */
    if (fgets(buffer, max_len, stdin) != (void *)0)
    {
        /* Strip trailing newline if present */
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
    {
        buffer[0] = '\0';
    }

    /* Switch back to raw mode */
    tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
}
