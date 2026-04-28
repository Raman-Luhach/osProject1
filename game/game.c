#include <stdio.h>
#include "../game/game.h"
#include "../math/math.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../screen/screen.h"
#include "../keyboard/keyboard.h"
#include "../timer/timer.h"
#include "../random/random.h"
#include "../audio/audio.h"

/* All constants come from config/config.h via game.h */

static const char *snake_gradient_color(int index, int length)
{
    int third = custom_divide(length, 3);

    if (third < 1)
        third = 1;
    if (index == 0)
        return COLOR_BRIGHT_GREEN;
    if (index < third)
        return COLOR_GREEN;
    if (index < custom_multiply(third, 2))
        return COLOR_CYAN;
    return COLOR_DIM;
}

void game_setup_levels(GameState *state)
{
    int i;
    int lv;

    int center_x = GAME_AREA_X + custom_divide(GAME_AREA_WIDTH, 2);
    int center_y = GAME_AREA_Y + custom_divide(GAME_AREA_HEIGHT, 2);

    for (lv = 0; lv < MAX_LEVELS; lv++)
    {
        state->levels[lv].wall_count = 0;
        state->levels[lv].wrap_around = 0;
        state->levels[lv].spawn_x = center_x;
        state->levels[lv].spawn_y = center_y;
    }

    /* Level 1: Wrap-around, no walls */
    state->levels[0].target_score = LEVEL_1_TARGET;
    state->levels[0].base_speed = LEVEL_1_SPEED;
    state->levels[0].wrap_around = 1;

    /* Level 2: Normal walls, no obstacles */
    state->levels[1].target_score = LEVEL_2_TARGET;
    state->levels[1].base_speed = LEVEL_2_SPEED;
    state->levels[1].wrap_around = 0;

    /* Level 3: Horizontal wall at y=14 (middle row) */
    state->levels[2].target_score = LEVEL_3_TARGET;
    state->levels[2].base_speed = LEVEL_3_SPEED;
    state->levels[2].wrap_around = 0;
    state->levels[2].spawn_x = GAME_AREA_X + 10;
    state->levels[2].spawn_y = GAME_AREA_Y + 4;
    for (i = 0; i < 15; i++)
    {
        state->levels[2].walls[i].x = GAME_AREA_X + 22 + i;
        state->levels[2].walls[i].y = GAME_AREA_Y + 10;
    }
    state->levels[2].wall_count = 15;

    /* Level 4: Two vertical walls + wrap-around */
    state->levels[3].target_score = LEVEL_4_TARGET;
    state->levels[3].base_speed = LEVEL_4_SPEED;
    state->levels[3].wrap_around = 1;
    state->levels[3].spawn_x = GAME_AREA_X + 30;
    state->levels[3].spawn_y = center_y;
    for (i = 0; i < 8; i++)
    {
        state->levels[3].walls[i].x = GAME_AREA_X + 15;
        state->levels[3].walls[i].y = GAME_AREA_Y + 6 + i;
    }
    for (i = 0; i < 8; i++)
    {
        state->levels[3].walls[8 + i].x = GAME_AREA_X + 44;
        state->levels[3].walls[8 + i].y = GAME_AREA_Y + 6 + i;
    }
    state->levels[3].wall_count = 16;

    /* Level 5: Box in center — spawn top-left, away from box */
    state->levels[4].target_score = LEVEL_5_TARGET;
    state->levels[4].base_speed = LEVEL_5_SPEED;
    state->levels[4].wrap_around = 0;
    state->levels[4].spawn_x = GAME_AREA_X + 8;
    state->levels[4].spawn_y = GAME_AREA_Y + 4;
    {
        int w = 0;
        for (i = 0; i < 8; i++)
        {
            state->levels[4].walls[w].x = GAME_AREA_X + 26 + i;
            state->levels[4].walls[w].y = GAME_AREA_Y + 8;
            w++;
        }
        for (i = 0; i < 8; i++)
        {
            state->levels[4].walls[w].x = GAME_AREA_X + 26 + i;
            state->levels[4].walls[w].y = GAME_AREA_Y + 13;
            w++;
        }
        for (i = 1; i < 5; i++)
        {
            state->levels[4].walls[w].x = GAME_AREA_X + 26;
            state->levels[4].walls[w].y = GAME_AREA_Y + 8 + i;
            w++;
            state->levels[4].walls[w].x = GAME_AREA_X + 33;
            state->levels[4].walls[w].y = GAME_AREA_Y + 8 + i;
            w++;
        }
        state->levels[4].wall_count = w;
    }
}

/* Repositions snake to the level's spawn point */
void game_apply_level(GameState *state, int level)
{
    int sx = state->levels[level].spawn_x;
    int sy = state->levels[level].spawn_y;

    state->current_level = level;
    state->frame_delay = state->levels[level].base_speed;

    state->snake.body[0].x = sx;
    state->snake.body[0].y = sy;
    state->snake.body[1].x = sx - 1;
    state->snake.body[1].y = sy;
    state->snake.body[2].x = sx - 2;
    state->snake.body[2].y = sy;
    state->snake.length = 3;
    state->snake.direction = DIR_RIGHT;

    state->old_tail.x = state->snake.body[2].x;
    state->old_tail.y = state->snake.body[2].y;
    state->trail_count = 0;
}

static int is_on_obstacle(GameState *state, int x, int y)
{
    int i;
    int lv = state->current_level;
    LevelData *level;

    if (lv < 0 || lv >= MAX_LEVELS)
        return 0;
    level = &state->levels[lv];

    for (i = 0; i < level->wall_count; i++)
    {
        if (level->walls[i].x == x && level->walls[i].y == y)
            return 1;
    }
    return 0;
}

static int is_on_snake(GameState *state, int x, int y)
{
    int i;

    for (i = 0; i < state->snake.length; i++)
    {
        if (state->snake.body[i].x == x && state->snake.body[i].y == y)
            return 1;
    }
    return 0;
}

static void place_food(GameState *state, Food *food, int type)
{
    int new_x;
    int new_y;
    int attempt;

    for (attempt = 0; attempt < 200; attempt++)
    {
        new_x = GAME_AREA_X + random_range(GAME_AREA_WIDTH);
        new_y = GAME_AREA_Y + random_range(GAME_AREA_HEIGHT);

        if (!is_on_snake(state, new_x, new_y)
            && !is_on_obstacle(state, new_x, new_y))
        {
            food->position.x = new_x;
            food->position.y = new_y;
            food->active = 1;
            food->type = type;
            food->spawn_time = timer_get_ms();

            if (type == FOOD_NORMAL)
                food->symbol = '*';
            else if (type == FOOD_BONUS)
                food->symbol = '$';
            else
                food->symbol = 'X';
            return;
        }
    }

    food->position.x = GAME_AREA_X + 1;
    food->position.y = GAME_AREA_Y + 1;
    food->active = 1;
    food->type = type;
    food->symbol = '*';
    food->spawn_time = timer_get_ms();
}

void game_init(GameState *state)
{
    int center_x;
    int center_y;
    int i;
    char *demo_block;

    demo_block = (char *)mem_alloc(32);
    if (demo_block != (void *)0)
    {
        demo_block[0] = 'V';
        demo_block[1] = 'A';
        demo_block[2] = 'S';
        demo_block[3] = 'U';
        demo_block[4] = 'K';
        demo_block[5] = 'I';
        demo_block[6] = '\0';
        mem_free(demo_block);
    }

    /* Default spawn at center — game_apply_level overrides this */
    center_x = GAME_AREA_X + custom_divide(GAME_AREA_WIDTH, 2);
    center_y = GAME_AREA_Y + custom_divide(GAME_AREA_HEIGHT, 2);

    state->snake.body[0].x = center_x;
    state->snake.body[0].y = center_y;
    state->snake.body[1].x = center_x - 1;
    state->snake.body[1].y = center_y;
    state->snake.body[2].x = center_x - 2;
    state->snake.body[2].y = center_y;
    state->snake.length = 3;
    state->snake.direction = DIR_RIGHT;
    state->snake.alive = 1;

    state->score = 0;
    state->running = 1;
    state->paused = 0;
    state->tick_count = 0;
    state->grew = 0;
    state->combo = 0;
    state->combo_multiplier = 1;
    state->last_food_time = timer_get_ms();
    state->frame_delay = BASE_FRAME_DELAY;
    state->trail_count = 0;
    state->current_level = 0;
    state->game_mode = MODE_ENDLESS;
    state->high_score = 0;
    state->direction_locked = 0;

    i = state->snake.length - 1;
    state->old_tail.x = state->snake.body[i].x;
    state->old_tail.y = state->snake.body[i].y;

    game_setup_levels(state);

    state->bonus_food.active = 0;
    state->bonus_food.type = FOOD_BONUS;

    place_food(state, &state->food, FOOD_NORMAL);
}

void game_process_input(GameState *state, int key)
{
    int current_dir = state->snake.direction;

    if (key == 'p' || key == 'P')
    {
        state->paused = !state->paused;
        return;
    }

    if (state->paused)
        return;

    /* Only allow one direction change per tick to prevent simultaneous key issues */
    if (state->direction_locked)
    {
        if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
            state->running = 0;
        return;
    }

    switch (key)
    {
        case 'w': case 'W': case KEY_UP:
            if (current_dir != DIR_DOWN)
            {
                state->snake.direction = DIR_UP;
                state->direction_locked = 1;
            }
            break;
        case 's': case 'S': case KEY_DOWN:
            if (current_dir != DIR_UP)
            {
                state->snake.direction = DIR_DOWN;
                state->direction_locked = 1;
            }
            break;
        case 'a': case 'A': case KEY_LEFT:
            if (current_dir != DIR_RIGHT)
            {
                state->snake.direction = DIR_LEFT;
                state->direction_locked = 1;
            }
            break;
        case 'd': case 'D': case KEY_RIGHT:
            if (current_dir != DIR_LEFT)
            {
                state->snake.direction = DIR_RIGHT;
                state->direction_locked = 1;
            }
            break;
        case 'q': case 'Q': case KEY_ESCAPE:
            state->running = 0;
            break;
        default:
            break;
    }
}

void game_update(GameState *state)
{
    int   i;
    int   head_x;
    int   head_y;
    Point *body;
    int   lv;
    LevelData *level;
    long  now;

    if (!state->snake.alive || state->paused)
        return;

    /* Unlock direction for next input */
    state->direction_locked = 0;

    body = state->snake.body;
    lv = state->current_level;
    level = &state->levels[lv];

    state->old_tail.x = body[state->snake.length - 1].x;
    state->old_tail.y = body[state->snake.length - 1].y;
    state->grew = 0;

    /* Trail buffer */
    if (state->trail_count < TRAIL_SIZE)
    {
        state->trail[state->trail_count] = state->old_tail;
        state->trail_count++;
    }
    else
    {
        for (i = 0; i < TRAIL_SIZE - 1; i++)
            state->trail[i] = state->trail[i + 1];
        state->trail[TRAIL_SIZE - 1] = state->old_tail;
    }

    /* Shift body */
    for (i = state->snake.length - 1; i > 0; i--)
    {
        body[i].x = body[i - 1].x;
        body[i].y = body[i - 1].y;
    }

    /* Move head */
    switch (state->snake.direction)
    {
        case DIR_UP:    body[0].y -= 1; break;
        case DIR_DOWN:  body[0].y += 1; break;
        case DIR_LEFT:  body[0].x -= 1; break;
        case DIR_RIGHT: body[0].x += 1; break;
    }

    head_x = body[0].x;
    head_y = body[0].y;

    /* Wall collision / wrap-around */
    if (level->wrap_around)
    {
        if (head_x < GAME_AREA_X)
            body[0].x = GAME_AREA_X + GAME_AREA_WIDTH - 1;
        else if (head_x >= GAME_AREA_X + GAME_AREA_WIDTH)
            body[0].x = GAME_AREA_X;
        if (head_y < GAME_AREA_Y)
            body[0].y = GAME_AREA_Y + GAME_AREA_HEIGHT - 1;
        else if (head_y >= GAME_AREA_Y + GAME_AREA_HEIGHT)
            body[0].y = GAME_AREA_Y;
        head_x = body[0].x;
        head_y = body[0].y;
    }
    else
    {
        if (!is_in_bounds(head_x, head_y,
                          GAME_AREA_X, GAME_AREA_Y,
                          GAME_AREA_X + GAME_AREA_WIDTH - 1,
                          GAME_AREA_Y + GAME_AREA_HEIGHT - 1))
        {
            state->snake.alive = 0;
            state->running = 0;
            audio_play(SOUND_DIE);
            return;
        }
    }

    /* Obstacle collision */
    if (is_on_obstacle(state, head_x, head_y))
    {
        state->snake.alive = 0;
        state->running = 0;
        audio_play(SOUND_DIE);
        return;
    }

    /* Self collision */
    for (i = 1; i < state->snake.length; i++)
    {
        if (head_x == body[i].x && head_y == body[i].y)
        {
            state->snake.alive = 0;
            state->running = 0;
            audio_play(SOUND_DIE);
            return;
        }
    }

    now = timer_get_ms();

    /* Normal food collision */
    if (head_x == state->food.position.x && head_y == state->food.position.y)
    {
        if (state->snake.length < MAX_SNAKE_LENGTH)
        {
            state->snake.body[state->snake.length].x = state->old_tail.x;
            state->snake.body[state->snake.length].y = state->old_tail.y;
            state->snake.length++;
            state->grew = 1;
        }

        /* Combo */
        if (now - state->last_food_time < COMBO_WINDOW_MS)
        {
            state->combo++;
            state->combo_multiplier = custom_min(state->combo + 1, MAX_COMBO_MULTIPLIER);
        }
        else
        {
            state->combo = 0;
            state->combo_multiplier = 1;
        }
        state->last_food_time = now;

        state->score += custom_multiply(SCORE_NORMAL_FOOD, state->combo_multiplier);
        audio_play(SOUND_EAT);
        place_food(state, &state->food, FOOD_NORMAL);

        /* Speed up based on score */
        {
            int speed_reduction = custom_multiply(
                custom_divide(state->score, 50), SPEED_INCREASE_PER_50_POINTS);
            state->frame_delay = custom_max(
                state->levels[lv].base_speed - speed_reduction, MIN_FRAME_DELAY);
        }
    }

    /* Bonus food collision */
    if (state->bonus_food.active
        && head_x == state->bonus_food.position.x
        && head_y == state->bonus_food.position.y)
    {
        state->score += custom_multiply(SCORE_BONUS_FOOD, state->combo_multiplier);
        audio_play(SOUND_BONUS);
        screen_draw_char(state->bonus_food.position.x,
                         state->bonus_food.position.y, ' ');
        state->bonus_food.active = 0;

        if (state->snake.length < MAX_SNAKE_LENGTH)
        {
            state->snake.body[state->snake.length].x = state->old_tail.x;
            state->snake.body[state->snake.length].y = state->old_tail.y;
            state->snake.length++;
            state->grew = 1;
        }
    }

    /* Spawn bonus food every ~100 ticks */
    if (!state->bonus_food.active && custom_modulo(state->tick_count, BONUS_SPAWN_INTERVAL) == 0
        && state->tick_count > 0)
    {
        place_food(state, &state->bonus_food, FOOD_BONUS);
    }

    /* Bonus food expires */
    if (state->bonus_food.active
        && timer_has_expired(state->bonus_food.spawn_time, BONUS_DURATION_MS))
    {
        screen_draw_char(state->bonus_food.position.x,
                         state->bonus_food.position.y, ' ');
        state->bonus_food.active = 0;
    }

    state->tick_count++;
}

int game_check_level_complete(GameState *state)
{
    int target;

    if (state->game_mode != MODE_LEVELS)
        return 0;

    target = state->levels[state->current_level].target_score;
    if (state->score >= target)
        return 1;
    return 0;
}

void game_render_border(GameState *state)
{
    int title_x;
    int info_y;
    int lv = state->current_level;

    title_x = GAME_AREA_X + custom_divide(GAME_AREA_WIDTH - 20, 2);
    screen_draw_string_color(title_x, 1,
        "=*= V A S U K I =*=", COLOR_BRIGHT_GREEN);

    if (state->game_mode == MODE_LEVELS)
    {
        char lvl_text[32];
        char lvl_num[4];
        str_copy(lvl_text, "Level ");
        int_to_string(lv + 1, lvl_num, 4);
        str_concat(lvl_text, lvl_num);
        if (state->levels[lv].wrap_around)
            str_concat(lvl_text, " [WRAP]");
        screen_draw_string_color(title_x - 2, 2, lvl_text, COLOR_BRIGHT_CYAN);
    }
    else
    {
        char lvl_text[32];
        char lvl_num[4];
        str_copy(lvl_text, "~- Endless ");
        if (lv > 0)
        {
            str_concat(lvl_text, "Stage ");
            int_to_string(lv + 1, lvl_num, 4);
            str_concat(lvl_text, lvl_num);
        }
        if (state->levels[lv].wrap_around)
            str_concat(lvl_text, " [WRAP]");
        str_concat(lvl_text, " -~");
        screen_draw_string_color(title_x - 4, 2, lvl_text, COLOR_BRIGHT_CYAN);
    }

    if (state->levels[lv].wrap_around)
        screen_draw_box_color(GAME_AREA_X - 1, GAME_AREA_Y - 1,
                              GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2,
                              COLOR_MAGENTA);
    else
        screen_draw_box_color(GAME_AREA_X - 1, GAME_AREA_Y - 1,
                              GAME_AREA_WIDTH + 2, GAME_AREA_HEIGHT + 2,
                              COLOR_CYAN);

    screen_draw_string_color(GAME_AREA_X + 1,
        GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
        "Score: 0         ", COLOR_BRIGHT_YELLOW);

    {
        char hs_text[32];
        char hs_num[16];
        str_copy(hs_text, "Best: ");
        int_to_string(state->high_score, hs_num, 16);
        str_concat(hs_text, hs_num);
        screen_draw_string_color(GAME_AREA_X + 25,
            GAME_AREA_Y + GAME_AREA_HEIGHT + 1, hs_text, COLOR_DIM);
    }

    screen_draw_string_color(GAME_AREA_X + GAME_AREA_WIDTH - 10,
        GAME_AREA_Y + GAME_AREA_HEIGHT + 1, "[Q] Quit", COLOR_DIM);

    info_y = GAME_AREA_Y + GAME_AREA_HEIGHT + 2;
    screen_draw_string_color(GAME_AREA_X, info_y,
        "[W/A/S/D] Move  [P] Pause  [Q] Quit", COLOR_DIM);
}

void game_render_score(GameState *state)
{
    char score_text[48];
    char score_num[16];
    char len_num[16];

    str_copy(score_text, "Score: ");
    int_to_string(state->score, score_num, 16);
    str_concat(score_text, score_num);
    str_concat(score_text, "  Len: ");
    int_to_string(state->snake.length, len_num, 16);
    str_concat(score_text, len_num);
    str_concat(score_text, "    ");

    screen_draw_string_color(GAME_AREA_X + 1,
        GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
        score_text, COLOR_BRIGHT_YELLOW);

    if (state->combo_multiplier > 1)
    {
        char combo_text[16];
        char combo_num[4];
        str_copy(combo_text, "x");
        int_to_string(state->combo_multiplier, combo_num, 4);
        str_concat(combo_text, combo_num);
        str_concat(combo_text, "! ");
        screen_draw_string_color(GAME_AREA_X + 45,
            GAME_AREA_Y + GAME_AREA_HEIGHT + 1,
            combo_text, COLOR_BRIGHT_MAGENTA);
    }
    else
    {
        screen_draw_string(GAME_AREA_X + 45,
            GAME_AREA_Y + GAME_AREA_HEIGHT + 1, "     ");
    }

    if (state->game_mode == MODE_LEVELS)
    {
        int target = state->levels[state->current_level].target_score;
        char target_text[32];
        char target_num[16];
        str_copy(target_text, "Goal: ");
        int_to_string(target, target_num, 16);
        str_concat(target_text, target_num);
        str_concat(target_text, "  ");
        screen_draw_string_color(GAME_AREA_X + 1,
            GAME_AREA_Y + GAME_AREA_HEIGHT + 3,
            target_text, COLOR_BRIGHT_CYAN);
    }
}

void game_render_obstacles(GameState *state)
{
    int i;
    LevelData *level = &state->levels[state->current_level];

    for (i = 0; i < level->wall_count; i++)
    {
        screen_draw_char_color(level->walls[i].x, level->walls[i].y,
                               '#', COLOR_RED);
    }
}

void game_render_full_snake(GameState *state)
{
    int i;

    screen_draw_char_color(state->snake.body[0].x, state->snake.body[0].y,
                           'O', COLOR_BRIGHT_GREEN);

    for (i = 1; i < state->snake.length; i++)
    {
        screen_draw_char_color(state->snake.body[i].x, state->snake.body[i].y,
                               'o', snake_gradient_color(i, state->snake.length));
    }
}

void game_render(GameState *state)
{
    int i;

    /* Draw trail dots */
    for (i = 0; i < state->trail_count; i++)
    {
        int tx = state->trail[i].x;
        int ty = state->trail[i].y;
        if (!is_on_snake(state, tx, ty) && !is_on_obstacle(state, tx, ty))
            screen_draw_char_color(tx, ty, '.', COLOR_DIM);
    }

    /* Erase oldest trail */
    if (state->trail_count >= TRAIL_SIZE)
    {
        int tx = state->trail[0].x;
        int ty = state->trail[0].y;
        if (!is_on_snake(state, tx, ty) && !is_on_obstacle(state, tx, ty))
            screen_draw_char(tx, ty, ' ');
    }

    /* Erase old tail */
    if (!state->grew)
        screen_draw_char(state->old_tail.x, state->old_tail.y, ' ');

    /* Body gradient */
    if (state->snake.length > 1)
    {
        screen_draw_char_color(state->snake.body[1].x,
                               state->snake.body[1].y, 'o',
                               snake_gradient_color(1, state->snake.length));
    }

    /* Head */
    screen_draw_char_color(state->snake.body[0].x,
                           state->snake.body[0].y, 'O', COLOR_BRIGHT_GREEN);

    /* Normal food blinks */
    if (state->food.active)
    {
        char food_ch = (custom_modulo(state->tick_count, 6) < 3) ? '*' : '+';
        screen_draw_char_color(state->food.position.x,
                               state->food.position.y,
                               food_ch, COLOR_BRIGHT_YELLOW);
    }

    /* Bonus food */
    if (state->bonus_food.active)
    {
        screen_draw_char_color(state->bonus_food.position.x,
                               state->bonus_food.position.y,
                               '$', COLOR_BRIGHT_MAGENTA);
    }

    /* Pause overlay */
    if (state->paused)
    {
        screen_draw_string_color(GAME_AREA_X + 23, GAME_AREA_Y + 10,
            "[ PAUSED ]", COLOR_BRIGHT_WHITE);
    }

    game_render_score(state);
    screen_flush();
}
