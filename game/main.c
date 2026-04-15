/* ========================================================================== */
/*                        VASUKI - TERMINAL SNAKE GAME                       */
/*                                                                           */
/*  Entry point for the Vasuki snake game.                                   */
/*  Handles: welcome screen, game loop, game over screen.                   */
/*                                                                           */
/*  Library Pipeline (every library is used):                                */
/*    keyboard.c -> captures input (non-blocking)                           */
/*    string.c   -> parses/formats text (score display, etc.)               */
/*    memory.c   -> custom allocator (initialized at startup, used in game) */
/*    math.c     -> boundary checks, arithmetic                              */
/*    screen.c   -> all terminal rendering                                   */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../math/math.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../screen/screen.h"
#include "../keyboard/keyboard.h"
#include "../game/game.h"

/* Frame delay in microseconds (100,000 us = 100 ms = ~10 FPS). */
#define FRAME_DELAY_US  100000

/* ========================================================================== */
/*                           WELCOME SCREEN                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  show_welcome_screen                                                      */
/*  Displays an attractive ASCII art title, game controls, and waits for    */
/*  the player to press ENTER before starting the game.                     */
/* -------------------------------------------------------------------------- */
static void show_welcome_screen(void)
{
    int key;
    int blink_on;

    screen_clear();
    screen_hide_cursor();

    /* ---- Large ASCII Art Title: VASUKI ---- */
    screen_draw_string_color(11, 2,
        "__     __    _    ____  _   _ _  ___ ___", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(11, 3,
        "\\ \\   / /   / \\  / ___|| | | | |/ /|_ _|", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(11, 4,
        " \\ \\ / /   / _ \\ \\___ \\| | | | ' /  | |", COLOR_GREEN);
    screen_draw_string_color(11, 5,
        "  \\ V /   / ___ \\ ___) | |_| | . \\  | |", COLOR_GREEN);
    screen_draw_string_color(11, 6,
        "   \\_/   /_/   \\_\\____/ \\___/|_|\\_\\|___|", COLOR_BRIGHT_GREEN);

    /* ---- Subtitle ---- */
    screen_draw_string_color(18, 8,
        "--- The Serpent King of Terminal ---", COLOR_BRIGHT_CYAN);

    /* ---- Decorative snake ---- */
    screen_draw_string_color(20, 10,
        "~-~-~-~=<@>", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(15, 11,
        "\"The king of serpents awaits...\"", COLOR_DIM);

    /* ---- Controls Box ---- */
    screen_draw_string_color(16, 13,
        "+================================+", COLOR_YELLOW);
    screen_draw_string_color(16, 14,
        "|                                |", COLOR_YELLOW);
    screen_draw_string_color(16, 15,
        "|   [W/A/S/D]  Move Vasuki       |", COLOR_YELLOW);
    screen_draw_string_color(16, 16,
        "|   [Arrows]   Also Move         |", COLOR_YELLOW);
    screen_draw_string_color(16, 17,
        "|   [Q / ESC]  Quit Game         |", COLOR_YELLOW);
    screen_draw_string_color(16, 18,
        "|                                |", COLOR_YELLOW);
    screen_draw_string_color(16, 19,
        "+================================+", COLOR_YELLOW);

    /* Overwrite the text inside the box with white for readability */
    screen_draw_string_color(20, 15,
        "[W/A/S/D]  Move Vasuki", COLOR_BRIGHT_WHITE);
    screen_draw_string_color(20, 16,
        "[Arrows]   Also Move", COLOR_BRIGHT_WHITE);
    screen_draw_string_color(20, 17,
        "[Q / ESC]  Quit Game", COLOR_BRIGHT_WHITE);

    /* ---- Team Credit ---- */
    screen_draw_string_color(17, 21,
        "Team Vasuki | OS Final Project", COLOR_DIM);

    /* ---- Blinking "Press ENTER" prompt ---- */
    /* Alternate between showing and hiding the prompt for a blink effect. */
    blink_on = 1;

    while (1)
    {
        /* Draw or erase the prompt based on blink state */
        if (blink_on)
        {
            screen_draw_string_color(19, 23,
                ">>> Press ENTER to start <<<", COLOR_BRIGHT_WHITE);
        }
        else
        {
            screen_draw_string(19, 23,
                "                            ");
        }
        screen_flush();

        /* Check for key press (non-blocking, check several times per blink) */
        {
            int check;
            for (check = 0; check < 5; check++)
            {
                key = keyboard_pressed();
                if (key == KEY_ENTER || key == '\r' || key == '\n')
                    return;
                if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
                {
                    screen_clear();
                    screen_show_cursor();
                    keyboard_restore();
                    exit(0);
                }
                usleep(100000);  /* 100ms between checks */
            }
        }

        /* Toggle blink every ~500ms (5 checks * 100ms) */
        blink_on = !blink_on;
    }
}

/* ========================================================================== */
/*                          GAME OVER SCREEN                                 */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  show_game_over_screen                                                    */
/*  Displays the final score and waits for the player to press a key.       */
/*  Returns 1 if the player wants to play again (ENTER), 0 to quit (Q).    */
/* -------------------------------------------------------------------------- */
static int show_game_over_screen(int final_score)
{
    int  key;
    char score_str[16];

    screen_clear();

    /* ---- Game Over ASCII Art ---- */
    screen_draw_string_color(12, 5,
        "  ____    _    __  __ _____    _____     _______ ____  ",
        COLOR_BRIGHT_RED);
    screen_draw_string_color(12, 6,
        " / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\ ",
        COLOR_BRIGHT_RED);
    screen_draw_string_color(12, 7,
        "| |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) |",
        COLOR_RED);
    screen_draw_string_color(12, 8,
        "| |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ < ",
        COLOR_RED);
    screen_draw_string_color(12, 9,
        " \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_\\",
        COLOR_BRIGHT_RED);

    /* ---- Final Score ---- */
    int_to_string(final_score, score_str, 16);

    screen_draw_string_color(26, 12, "Your Final Score: ", COLOR_BRIGHT_YELLOW);
    screen_draw_string_color(44, 12, score_str, COLOR_BRIGHT_WHITE);

    /* ---- Decorative dead snake ---- */
    screen_draw_string_color(24, 14,
        "x~-~-~-~=<X>  Vasuki has fallen...", COLOR_DIM);

    /* ---- Options ---- */
    screen_draw_string_color(20, 17,
        "[ENTER]  Play Again", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(20, 18,
        "[Q]      Quit", COLOR_BRIGHT_RED);

    screen_flush();

    /* Wait for player decision */
    while (1)
    {
        key = keyboard_pressed();
        if (key == KEY_ENTER || key == '\r' || key == '\n')
            return 1;   /* Play again */
        if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
            return 0;   /* Quit */
        usleep(50000);  /* 50ms polling delay */
    }
}

/* ========================================================================== */
/*                             MAIN ENTRY                                    */
/* ========================================================================== */

int main(void)
{
    GameState state;
    int       key;
    int       play_again;

    /* ---- Initialize Core Systems ---- */
    mem_init();          /* Set up the custom memory allocator   */
    keyboard_init();     /* Switch terminal to raw mode          */
    atexit(keyboard_restore);  /* Safety net: always restore terminal  */

    /* ---- Welcome Screen ---- */
    show_welcome_screen();

    /* ---- Main Game Loop (supports replay) ---- */
    play_again = 1;
    while (play_again)
    {
        /* Initialize a fresh game */
        game_init(&state);

        /* Set up the screen */
        screen_clear();
        screen_hide_cursor();
        game_render_border();
        game_render_full_snake(&state);

        /* Draw initial food */
        screen_draw_char_color(state.food.position.x,
                               state.food.position.y,
                               state.food.symbol,
                               COLOR_BRIGHT_YELLOW);
        screen_flush();

        /* ---- Frame Loop ---- */
        /* Each iteration: read input -> update state -> render -> wait      */
        while (state.running)
        {
            /* Read keyboard (non-blocking) */
            key = keyboard_pressed();
            if (key != KEY_NONE)
                game_process_input(&state, key);

            /* Advance game by one tick */
            game_update(&state);

            /* Draw only what changed */
            if (state.running)
                game_render(&state);

            /* Wait for next frame (~10 FPS) */
            usleep(FRAME_DELAY_US);
        }

        /* ---- Game Over ---- */
        usleep(500000);  /* Brief pause before showing game over */
        play_again = show_game_over_screen(state.score);
    }

    /* ---- Cleanup ---- */
    screen_clear();
    screen_show_cursor();
    screen_move_cursor(1, 1);
    screen_draw_string_color(1, 1,
        "Thanks for playing Vasuki! Goodbye.", COLOR_BRIGHT_CYAN);
    screen_move_cursor(1, 3);
    screen_flush();
    keyboard_restore();

    return 0;
}
