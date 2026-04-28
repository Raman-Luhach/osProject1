#ifndef GAME_H
#define GAME_H

#include "../config/config.h"

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

#define MODE_ENDLESS  0
#define MODE_LEVELS   1

#define FOOD_NORMAL  0
#define FOOD_BONUS   1
#define FOOD_POISON  2

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    Point body[MAX_SNAKE_LENGTH];
    int   length;
    int   direction;
    int   alive;
} Snake;

typedef struct
{
    Point position;
    int   active;
    char  symbol;
    int   type;
    long  spawn_time;
} Food;

typedef struct
{
    int   wall_count;
    Point walls[MAX_OBSTACLES];
    int   target_score;
    int   base_speed;
    int   wrap_around;
    int   spawn_x;
    int   spawn_y;
} LevelData;

typedef struct
{
    Snake     snake;
    Food      food;
    Food      bonus_food;
    int       score;
    int       high_score;
    int       running;
    int       paused;
    int       tick_count;
    Point     old_tail;
    int       grew;
    int       game_mode;
    int       current_level;
    int       combo;
    int       combo_multiplier;
    long      last_food_time;
    int       frame_delay;
    Point     trail[TRAIL_SIZE];
    int       trail_count;
    int       direction_locked;
    LevelData levels[MAX_LEVELS];
} GameState;

void game_init(GameState *state);
void game_process_input(GameState *state, int key);
void game_update(GameState *state);
void game_render(GameState *state);
void game_render_border(GameState *state);
void game_render_score(GameState *state);
void game_render_full_snake(GameState *state);
void game_render_obstacles(GameState *state);
void game_setup_levels(GameState *state);
void game_apply_level(GameState *state, int level);
int  game_check_level_complete(GameState *state);

#endif
