#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../math/math.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../screen/screen.h"
#include "../keyboard/keyboard.h"
#include "../game/game.h"
#include "../timer/timer.h"
#include "../random/random.h"
#include "../fileio/fileio.h"
#include "../audio/audio.h"

/* All config constants come from config/config.h via game.h */

static int get_frame_delay(GameState *state)
{
    int delay = state->frame_delay;

    if (state->snake.direction == DIR_UP || state->snake.direction == DIR_DOWN)
        delay = custom_divide(custom_multiply(delay, VERTICAL_SPEED_FACTOR_NUM), VERTICAL_SPEED_FACTOR_DEN);

    return delay;
}

static int load_high_score(void)
{
    int hs = 0;
    file_load_int(HIGH_SCORE_FILE, &hs);
    return hs;
}

static void save_high_score(int score)
{
    file_save_int(HIGH_SCORE_FILE, score);
}

static void show_welcome_screen(void)
{
    int key;
    int blink_on = 1;

    screen_clear();
    screen_hide_cursor();

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

    screen_draw_string_color(18, 8,
        "--- The Serpent King of Terminal ---", COLOR_BRIGHT_CYAN);

    screen_draw_string_color(20, 10,
        "~-~-~-~=<@>", COLOR_BRIGHT_GREEN);

    screen_draw_string_color(16, 12,
        "+================================+", COLOR_YELLOW);
    screen_draw_string_color(16, 13,
        "|                                |", COLOR_YELLOW);
    screen_draw_string_color(16, 14,
        "|   [W/A/S/D]  Move Vasuki       |", COLOR_YELLOW);
    screen_draw_string_color(16, 15,
        "|   [Arrows]   Also Move         |", COLOR_YELLOW);
    screen_draw_string_color(16, 16,
        "|   [P]        Pause             |", COLOR_YELLOW);
    screen_draw_string_color(16, 17,
        "|   [Q / ESC]  Quit Game         |", COLOR_YELLOW);
    screen_draw_string_color(16, 18,
        "|                                |", COLOR_YELLOW);
    screen_draw_string_color(16, 19,
        "+================================+", COLOR_YELLOW);

    screen_draw_string_color(20, 14,
        "[W/A/S/D]  Move Vasuki", COLOR_BRIGHT_WHITE);
    screen_draw_string_color(20, 15,
        "[Arrows]   Also Move", COLOR_BRIGHT_WHITE);
    screen_draw_string_color(20, 16,
        "[P]        Pause", COLOR_BRIGHT_WHITE);
    screen_draw_string_color(20, 17,
        "[Q / ESC]  Quit Game", COLOR_BRIGHT_WHITE);

    {
        int hs = load_high_score();
        if (hs > 0)
        {
            char hs_text[32];
            char hs_num[16];
            str_copy(hs_text, "High Score: ");
            int_to_string(hs, hs_num, 16);
            str_concat(hs_text, hs_num);
            screen_draw_string_color(22, 21, hs_text, COLOR_BRIGHT_YELLOW);
        }
    }

    screen_draw_string_color(17, 23,
        "Team Vasuki | OS Final Project", COLOR_DIM);

    while (1)
    {
        if (blink_on)
            screen_draw_string_color(19, 25,
                ">>> Press ENTER to start <<<", COLOR_BRIGHT_WHITE);
        else
            screen_draw_string(19, 25,
                "                            ");
        screen_flush();

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
                usleep(100000);
            }
        }
        blink_on = !blink_on;
    }
}

static int show_mode_select(void)
{
    int key;

    screen_clear();
    screen_hide_cursor();

    screen_draw_string_color(20, 5,
        "=*= SELECT GAME MODE =*=", COLOR_BRIGHT_GREEN);

    screen_draw_string_color(18, 9,
        "[1]  ENDLESS MODE", COLOR_BRIGHT_CYAN);
    screen_draw_string_color(18, 10,
        "     Pick a level, speed increases over time.", COLOR_DIM);
    screen_draw_string_color(18, 11,
        "     Survive as long as you can!", COLOR_DIM);

    screen_draw_string_color(18, 14,
        "[2]  LEVEL MODE", COLOR_BRIGHT_YELLOW);
    screen_draw_string_color(18, 15,
        "     5 levels with obstacles and goals.", COLOR_DIM);
    screen_draw_string_color(18, 16,
        "     Reach the target score to advance!", COLOR_DIM);

    screen_draw_string_color(18, 19,
        "Press 1 or 2 to choose...", COLOR_BRIGHT_WHITE);
    screen_flush();

    while (1)
    {
        key = keyboard_pressed();
        if (key == '1')
            return MODE_ENDLESS;
        if (key == '2')
            return MODE_LEVELS;
        if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
        {
            screen_clear();
            screen_show_cursor();
            keyboard_restore();
            exit(0);
        }
        usleep(50000);
    }
}

/* Let the player pick which level layout to play in endless mode */
static int show_endless_level_select(void)
{
    int key;

    screen_clear();
    screen_hide_cursor();

    screen_draw_string_color(18, 4,
        "=*= CHOOSE YOUR ARENA =*=", COLOR_BRIGHT_GREEN);

    screen_draw_string_color(14, 7,
        "[1]  Wrap Around", COLOR_BRIGHT_CYAN);
    screen_draw_string_color(14, 8,
        "     No walls - go through edges. Easy start.", COLOR_DIM);

    screen_draw_string_color(14, 10,
        "[2]  Classic Walls", COLOR_BRIGHT_YELLOW);
    screen_draw_string_color(14, 11,
        "     Hit a wall and you die. No obstacles.", COLOR_DIM);

    screen_draw_string_color(14, 13,
        "[3]  Horizontal Barrier", COLOR_GREEN);
    screen_draw_string_color(14, 14,
        "     A wall cuts across the middle.", COLOR_DIM);

    screen_draw_string_color(14, 16,
        "[4]  Twin Pillars + Wrap", COLOR_MAGENTA);
    screen_draw_string_color(14, 17,
        "     Two vertical walls with wrap-around.", COLOR_DIM);

    screen_draw_string_color(14, 19,
        "[5]  The Box", COLOR_BRIGHT_RED);
    screen_draw_string_color(14, 20,
        "     Box obstacle in center. No wrap. Fast.", COLOR_DIM);

    screen_draw_string_color(18, 23,
        "Press 1-5 to choose...", COLOR_BRIGHT_WHITE);
    screen_flush();

    while (1)
    {
        key = keyboard_pressed();
        if (key >= '1' && key <= '5')
            return key - '1';
        if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
        {
            screen_clear();
            screen_show_cursor();
            keyboard_restore();
            exit(0);
        }
        usleep(50000);
    }
}

static void show_death_flash(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        screen_draw_box_color(GAME_AREA_X - 1, GAME_AREA_Y - 1,
                              GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2,
                              COLOR_BRIGHT_RED);
        screen_flush();
        usleep(80000);
        screen_draw_box_color(GAME_AREA_X - 1, GAME_AREA_Y - 1,
                              GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2,
                              COLOR_CYAN);
        screen_flush();
        usleep(80000);
    }
}

static void show_level_complete(int level)
{
    char msg[32];
    char num[4];

    str_copy(msg, "LEVEL ");
    int_to_string(level + 1, num, 4);
    str_concat(msg, num);
    str_concat(msg, " COMPLETE!");

    screen_draw_string_color(GAME_AREA_X + 18, GAME_AREA_Y + 9,
        "+==================+", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(GAME_AREA_X + 18, GAME_AREA_Y + 10,
        "|                  |", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(GAME_AREA_X + 20, GAME_AREA_Y + 10,
        msg, COLOR_BRIGHT_YELLOW);
    screen_draw_string_color(GAME_AREA_X + 18, GAME_AREA_Y + 11,
        "+==================+", COLOR_BRIGHT_GREEN);
    screen_flush();
    audio_play("sounds/level.wav");
    usleep(2000000);
}

static int show_game_over_screen(int final_score, int high_score)
{
    int  key;
    char score_str[16];
    char hs_str[16];

    screen_clear();

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

    int_to_string(final_score, score_str, 16);
    int_to_string(high_score, hs_str, 16);

    screen_draw_string_color(26, 12, "Your Score: ", COLOR_BRIGHT_YELLOW);
    screen_draw_string_color(38, 12, score_str, COLOR_BRIGHT_WHITE);

    screen_draw_string_color(26, 13, "High Score: ", COLOR_DIM);
    screen_draw_string_color(38, 13, hs_str, COLOR_BRIGHT_YELLOW);

    if (final_score >= high_score && final_score > 0)
        screen_draw_string_color(26, 14,
            "*** NEW HIGH SCORE! ***", COLOR_BRIGHT_MAGENTA);

    screen_draw_string_color(24, 16,
        "x~-~-~-~=<X>  Vasuki has fallen...", COLOR_DIM);

    screen_draw_string_color(20, 19,
        "[ENTER]  Play Again", COLOR_BRIGHT_GREEN);
    screen_draw_string_color(20, 20,
        "[Q]      Quit", COLOR_BRIGHT_RED);
    screen_flush();

    while (1)
    {
        key = keyboard_pressed();
        if (key == KEY_ENTER || key == '\r' || key == '\n')
            return 1;
        if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
            return 0;
        usleep(50000);
    }
}

int main(void)
{
    GameState state;
    int       key;
    int       play_again;
    int       game_mode;
    int       high_score;
    int       level_result;
    int       chosen_level;

    mem_init();
    keyboard_init();
    random_init();
    atexit(keyboard_restore);

    high_score = load_high_score();

    show_welcome_screen();

    play_again = 1;
    while (play_again)
    {
        game_mode = show_mode_select();

        /* For endless mode, let player pick the arena */
        chosen_level = 0;
        if (game_mode == MODE_ENDLESS)
            chosen_level = show_endless_level_select();

        game_init(&state);
        state.game_mode = game_mode;
        state.high_score = high_score;
        game_apply_level(&state, chosen_level);

        screen_clear();
        screen_hide_cursor();
        game_render_border(&state);
        game_render_obstacles(&state);
        game_render_full_snake(&state);

        screen_draw_char_color(state.food.position.x,
                               state.food.position.y,
                               state.food.symbol,
                               COLOR_BRIGHT_YELLOW);
        screen_flush();

        while (state.running)
        {
            key = keyboard_pressed();
            if (key != KEY_NONE)
                game_process_input(&state, key);

            game_update(&state);

            level_result = game_check_level_complete(&state);

            if (level_result == 1)
            {
                /* Level mode: advance to next level */
                show_level_complete(state.current_level);
                state.current_level++;

                if (state.current_level >= MAX_LEVELS)
                {
                    state.running = 0;
                    break;
                }

                /* Reset snake position for new level, keep score */
                {
                    int saved_score = state.score;
                    int saved_hs = state.high_score;
                    int saved_mode = state.game_mode;
                    int next_level = state.current_level;

                    game_init(&state);
                    state.score = saved_score;
                    state.high_score = saved_hs;
                    state.game_mode = saved_mode;
                    game_apply_level(&state, next_level);
                }

                screen_clear();
                screen_hide_cursor();
                game_render_border(&state);
                game_render_obstacles(&state);
                game_render_full_snake(&state);
                screen_flush();
            }

            if (state.running)
                game_render(&state);

            timer_sleep_us(get_frame_delay(&state));
        }

        if (state.score > high_score)
        {
            high_score = state.score;
            save_high_score(high_score);
        }

        if (!state.snake.alive)
        {
            show_death_flash();
            usleep(300000);
        }

        play_again = show_game_over_screen(state.score, high_score);
    }

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
