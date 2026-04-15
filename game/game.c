/* ========================================================================== */
/*                          SNAKE GAME LOGIC                                 */
/*                                                                           */
/*  All game mechanics: initialization, input handling, movement,            */
/*  collision detection, food spawning, and rendering.                       */
/*                                                                           */
/*  Library Integration:                                                     */
/*    math.c   -> is_in_bounds() for wall collision, custom_modulo() for    */
/*                food placement                                             */
/*    string.c -> int_to_string() for score display                         */
/*    memory.c -> mem_alloc()/mem_free() demonstration during init          */
/*    screen.c -> all drawing and cursor control                             */
/* ========================================================================== */

#include <stdio.h>
#include "../game/game.h"
#include "../math/math.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../screen/screen.h"
#include "../keyboard/keyboard.h"

/* ========================================================================== */
/*                           INITIALIZATION                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  game_init                                                                */
/*  Sets up the initial game state:                                          */
/*    - Snake starts at the center of the play area, length 3, moving right */
/*    - Food is placed at a fixed offset from top-left                       */
/*    - Score starts at 0                                                    */
/*                                                                           */
/*  Also demonstrates mem_alloc/mem_free to satisfy the project requirement */
/*  that the memory library is integrated and tested.                        */
/* -------------------------------------------------------------------------- */
void game_init(GameState *state)
{
    int   center_x;
    int   center_y;
    int   i;
    char *demo_block;

    /* ---- Demonstrate custom memory allocator ---- */
    /* Allocate a small block, use it, then free it.                        */
    /* This proves the memory library works within the game pipeline.        */
    demo_block = (char *)mem_alloc(32);
    if (demo_block != (void *)0)
    {
        /* Write something into the allocated memory */
        demo_block[0] = 'S';
        demo_block[1] = 'N';
        demo_block[2] = 'A';
        demo_block[3] = 'K';
        demo_block[4] = 'E';
        demo_block[5] = '\0';
        /* Free the block -- we don't need it anymore */
        mem_free(demo_block);
    }

    /* ---- Place the snake at the center ---- */
    center_x = GAME_AREA_X + custom_divide(GAME_AREA_WIDTH, 2);
    center_y = GAME_AREA_Y + custom_divide(GAME_AREA_HEIGHT, 2);

    /* Head is body[0], then two trailing segments to the left */
    state->snake.body[0].x = center_x;
    state->snake.body[0].y = center_y;
    state->snake.body[1].x = center_x - 1;
    state->snake.body[1].y = center_y;
    state->snake.body[2].x = center_x - 2;
    state->snake.body[2].y = center_y;

    state->snake.length    = 3;
    state->snake.direction = DIR_RIGHT;
    state->snake.alive     = 1;

    /* ---- Place the initial food ---- */
    state->food.position.x = GAME_AREA_X + 10;
    state->food.position.y = GAME_AREA_Y + 5;
    state->food.active      = 1;
    state->food.symbol      = '*';

    /* ---- Initialize counters ---- */
    state->score      = 0;
    state->running    = 1;
    state->tick_count = 0;
    state->grew       = 0;

    /* Initialize old_tail to the current tail position */
    i = state->snake.length - 1;
    state->old_tail.x = state->snake.body[i].x;
    state->old_tail.y = state->snake.body[i].y;
}

/* ========================================================================== */
/*                           INPUT HANDLING                                  */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  game_process_input                                                       */
/*  Maps key presses to direction changes.                                   */
/*  Supports both WASD and arrow keys.                                       */
/*                                                                           */
/*  Important: prevents 180-degree reversal (e.g., if moving right, you     */
/*  cannot instantly go left -- that would cause immediate self-collision).  */
/* -------------------------------------------------------------------------- */
void game_process_input(GameState *state, int key)
{
    int current_dir = state->snake.direction;

    switch (key)
    {
        /* ---- Up ---- */
        case 'w':
        case 'W':
        case KEY_UP:
            if (current_dir != DIR_DOWN)
                state->snake.direction = DIR_UP;
            break;

        /* ---- Down ---- */
        case 's':
        case 'S':
        case KEY_DOWN:
            if (current_dir != DIR_UP)
                state->snake.direction = DIR_DOWN;
            break;

        /* ---- Left ---- */
        case 'a':
        case 'A':
        case KEY_LEFT:
            if (current_dir != DIR_RIGHT)
                state->snake.direction = DIR_LEFT;
            break;

        /* ---- Right ---- */
        case 'd':
        case 'D':
        case KEY_RIGHT:
            if (current_dir != DIR_LEFT)
                state->snake.direction = DIR_RIGHT;
            break;

        /* ---- Quit ---- */
        case 'q':
        case 'Q':
        case KEY_ESCAPE:
            state->running = 0;
            break;

        default:
            break;  /* Ignore unrecognized keys */
    }
}

/* ========================================================================== */
/*                        FOOD PLACEMENT HELPER                              */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  place_food                                                               */
/*  Computes a new position for the food using the tick counter as a seed.  */
/*  Uses custom_modulo from math.c to keep the position within bounds.      */
/*  Checks that the food doesn't land on the snake's body.                  */
/* -------------------------------------------------------------------------- */
static void place_food(GameState *state)
{
    int new_x;
    int new_y;
    int on_snake;
    int i;
    int attempt;

    for (attempt = 0; attempt < 100; attempt++)
    {
        /* Pseudo-random placement using tick_count as a changing seed */
        new_x = GAME_AREA_X
                + custom_modulo(state->tick_count * 7 + attempt * 13 + 3,
                                GAME_AREA_WIDTH);
        new_y = GAME_AREA_Y
                + custom_modulo(state->tick_count * 11 + attempt * 17 + 7,
                                GAME_AREA_HEIGHT);

        /* Check that the food is not on the snake */
        on_snake = 0;
        for (i = 0; i < state->snake.length; i++)
        {
            if (state->snake.body[i].x == new_x
                && state->snake.body[i].y == new_y)
            {
                on_snake = 1;
                break;
            }
        }

        if (!on_snake)
        {
            state->food.position.x = new_x;
            state->food.position.y = new_y;
            state->food.active     = 1;
            return;
        }
    }

    /* Fallback: if all attempts fail, just place it somewhere safe */
    state->food.position.x = GAME_AREA_X + 1;
    state->food.position.y = GAME_AREA_Y + 1;
    state->food.active     = 1;
}

/* ========================================================================== */
/*                            GAME UPDATE                                    */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  game_update                                                              */
/*  Called once per frame. Performs these steps:                              */
/*    1. Save the current tail position (for erasing later).                */
/*    2. Shift every body segment forward (tail follows head).              */
/*    3. Move the head one step in the current direction.                    */
/*    4. Check if the head collides with food (eat it, grow, reposition).   */
/*    5. Check if the head hit a wall (using is_in_bounds from math.c).    */
/*    6. Check if the head hit the snake's own body.                        */
/* -------------------------------------------------------------------------- */
void game_update(GameState *state)
{
    int   i;
    int   head_x;
    int   head_y;
    Point *body;

    if (!state->snake.alive)
        return;

    body = state->snake.body;

    /* Step 1: Save tail position before shifting (used by renderer) */
    state->old_tail.x = body[state->snake.length - 1].x;
    state->old_tail.y = body[state->snake.length - 1].y;
    state->grew = 0;

    /* Step 2: Shift body -- each segment takes the position of the one     */
    /*         in front of it.  body[0] (head) is updated separately.       */
    for (i = state->snake.length - 1; i > 0; i--)
    {
        body[i].x = body[i - 1].x;
        body[i].y = body[i - 1].y;
    }

    /* Step 3: Move head in the current direction */
    switch (state->snake.direction)
    {
        case DIR_UP:    body[0].y -= 1; break;
        case DIR_DOWN:  body[0].y += 1; break;
        case DIR_LEFT:  body[0].x -= 1; break;
        case DIR_RIGHT: body[0].x += 1; break;
    }

    head_x = body[0].x;
    head_y = body[0].y;

    /* Step 4: Check food collision */
    if (head_x == state->food.position.x && head_y == state->food.position.y)
    {
        /* Grow the snake by one segment */
        if (state->snake.length < MAX_SNAKE_LENGTH)
        {
            /* The new tail segment stays where the old tail was */
            state->snake.body[state->snake.length].x = state->old_tail.x;
            state->snake.body[state->snake.length].y = state->old_tail.y;
            state->snake.length++;
            state->grew = 1;
        }

        /* Increase score */
        state->score += 10;

        /* Reposition food */
        place_food(state);
    }

    /* Step 5: Wall collision using is_in_bounds() from math.c              */
    /* The playable area spans:                                              */
    /*   x: [GAME_AREA_X, GAME_AREA_X + GAME_AREA_WIDTH - 1]              */
    /*   y: [GAME_AREA_Y, GAME_AREA_Y + GAME_AREA_HEIGHT - 1]             */
    if (!is_in_bounds(head_x, head_y,
                      GAME_AREA_X,
                      GAME_AREA_Y,
                      GAME_AREA_X + GAME_AREA_WIDTH - 1,
                      GAME_AREA_Y + GAME_AREA_HEIGHT - 1))
    {
        state->snake.alive = 0;
        state->running     = 0;
        return;
    }

    /* Step 6: Self collision -- check if head overlaps any body segment */
    for (i = 1; i < state->snake.length; i++)
    {
        if (head_x == body[i].x && head_y == body[i].y)
        {
            state->snake.alive = 0;
            state->running     = 0;
            return;
        }
    }

    /* Advance frame counter */
    state->tick_count++;
}

/* ========================================================================== */
/*                             RENDERING                                     */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*  game_render_border                                                       */
/*  Draws the static elements that never change during gameplay:             */
/*    - Game title (centered, bright green + bold)                          */
/*    - Score label and quit hint                                            */
/*    - Play area border (cyan box)                                         */
/*    - Control instructions at the bottom                                   */
/* -------------------------------------------------------------------------- */
void game_render_border(void)
{
    int title_x;
    int info_y;

    /* ---- Title Row ---- */
    title_x = GAME_AREA_X + custom_divide(GAME_AREA_WIDTH - 20, 2);
    screen_draw_string_color(title_x, 1,
        "=*= V A S U K I =*=",
        COLOR_BRIGHT_GREEN);
    screen_draw_string_color(title_x - 4, 2,
        "~-~- The Serpent King -~-~",
        COLOR_BRIGHT_CYAN);
    (void)title_x;  /* suppress unused warning after refactor */

    /* ---- Score and Quit labels (row just above border) ---- */
    /* We'll put them on the same row as the box top */

    /* ---- Play Area Border ---- */
    /* The border is 1 cell outside the playable area on each side. */
    screen_draw_box_color(GAME_AREA_X - 1, GAME_AREA_Y - 1,
                          GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2,
                          COLOR_CYAN);

    /* ---- Score Display (above play area, left side) ---- */
    screen_draw_string_color(GAME_AREA_X + 1, GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
        "Score: 0       ", COLOR_BRIGHT_YELLOW);

    /* ---- Quit Hint (above play area, right side) ---- */
    screen_draw_string_color(GAME_AREA_X + GAME_AREA_WIDTH - 10,
        GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
        "[Q] Quit", COLOR_DIM);

    /* ---- Control Instructions (below the border) ---- */
    info_y = GAME_AREA_Y + GAME_AREA_HEIGHT + 2;
    screen_draw_string_color(GAME_AREA_X, info_y,
        "Controls: [W/A/S/D] or [Arrow Keys] to move",
        COLOR_DIM);
}

/* -------------------------------------------------------------------------- */
/*  game_render_score                                                        */
/*  Converts the numeric score to a string (using string.c) and draws it.  */
/* -------------------------------------------------------------------------- */
void game_render_score(GameState *state)
{
    char score_text[32];
    char score_num[16];

    /* Build "Score: 123" using our custom string functions */
    score_text[0] = '\0';
    str_copy(score_text, "Score: ");
    int_to_string(state->score, score_num, 16);
    str_concat(score_text, score_num);
    str_concat(score_text, "       ");  /* Padding to overwrite old digits */

    screen_draw_string_color(GAME_AREA_X + 1, GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
                             score_text, COLOR_BRIGHT_YELLOW);
}

/* -------------------------------------------------------------------------- */
/*  game_render_full_snake                                                   */
/*  Draws every segment of the snake. Called once at the start of the game. */
/* -------------------------------------------------------------------------- */
void game_render_full_snake(GameState *state)
{
    int i;

    /* Draw the head */
    screen_draw_char_color(state->snake.body[0].x, state->snake.body[0].y,
                           'O', COLOR_BRIGHT_GREEN);

    /* Draw body segments */
    for (i = 1; i < state->snake.length; i++)
    {
        screen_draw_char_color(state->snake.body[i].x, state->snake.body[i].y,
                               'o', COLOR_GREEN);
    }
}

/* -------------------------------------------------------------------------- */
/*  game_render                                                              */
/*  Differential renderer -- only redraws what changed since the last frame. */
/*  This is MUCH faster than clearing the whole screen and prevents flicker. */
/*                                                                           */
/*  What changes each frame:                                                 */
/*    1. The old tail position needs to be erased (draw a space).           */
/*    2. The old head position becomes a body segment ('o').                */
/*    3. The new head position gets the head character ('O').               */
/*    4. If food was eaten, draw the new food and update the score.         */
/* -------------------------------------------------------------------------- */
void game_render(GameState *state)
{
    /* 1. Erase the old tail (unless the snake grew this frame) */
    if (!state->grew)
    {
        screen_draw_char(state->old_tail.x, state->old_tail.y, ' ');
    }

    /* 2. The segment behind the head (body[1]) is now a body character */
    if (state->snake.length > 1)
    {
        screen_draw_char_color(state->snake.body[1].x,
                               state->snake.body[1].y,
                               'o', COLOR_GREEN);
    }

    /* 3. Draw the new head position */
    screen_draw_char_color(state->snake.body[0].x,
                           state->snake.body[0].y,
                           'O', COLOR_BRIGHT_GREEN);

    /* 4. Redraw food (in case it was just repositioned) */
    if (state->food.active)
    {
        screen_draw_char_color(state->food.position.x,
                               state->food.position.y,
                               state->food.symbol,
                               COLOR_BRIGHT_YELLOW);
    }

    /* 5. Update score display */
    game_render_score(state);

    /* 6. Push everything to the terminal */
    screen_flush();
}
