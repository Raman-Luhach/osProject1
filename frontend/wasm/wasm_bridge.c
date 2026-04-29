/* Bridge between C game logic and JavaScript frontend.
   Exposes game state and functions to JS via Emscripten exports. */

#include <emscripten.h>
#include "../../math/math.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../game/game.h"
#include "../../timer/timer.h"
#include "../../random/random.h"
#include "../../fileio/fileio.h"

static GameState g_state;
static int g_initialized = 0;

/* ---- Initialization ---- */

EMSCRIPTEN_KEEPALIVE
void wasm_init(void)
{
    if (!g_initialized)
    {
        mem_init();
        random_init();
        g_initialized = 1;
    }
    game_init(&g_state);
}

EMSCRIPTEN_KEEPALIVE
void wasm_setup_game(int mode, int level)
{
    game_init(&g_state);
    g_state.game_mode = mode;
    g_state.high_score = 0;
    game_apply_level(&g_state, level);
}

EMSCRIPTEN_KEEPALIVE
void wasm_set_high_score(int hs)
{
    g_state.high_score = hs;
}

/* ---- Input ---- */

EMSCRIPTEN_KEEPALIVE
void wasm_process_input(int key)
{
    game_process_input(&g_state, key);
}

EMSCRIPTEN_KEEPALIVE
void wasm_toggle_pause(void)
{
    g_state.paused = !g_state.paused;
}

/* ---- Update ---- */

EMSCRIPTEN_KEEPALIVE
void wasm_update(void)
{
    game_update(&g_state);
}

EMSCRIPTEN_KEEPALIVE
int wasm_check_level_complete(void)
{
    return game_check_level_complete(&g_state);
}

EMSCRIPTEN_KEEPALIVE
void wasm_advance_level(void)
{
    int saved_score = g_state.score;
    int saved_hs = g_state.high_score;
    int saved_mode = g_state.game_mode;
    int next_level = g_state.current_level + 1;

    game_init(&g_state);
    g_state.score = saved_score;
    g_state.high_score = saved_hs;
    g_state.game_mode = saved_mode;
    game_apply_level(&g_state, next_level);
}

/* ---- State Getters (called by JS to read game state for rendering) ---- */

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_length(void) { return g_state.snake.length; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_x(int index) { return g_state.snake.body[index].x - GAME_AREA_X; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_y(int index) { return g_state.snake.body[index].y - GAME_AREA_Y; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_direction(void) { return g_state.snake.direction; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_alive(void) { return g_state.snake.alive; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_food_x(void) { return g_state.food.position.x - GAME_AREA_X; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_food_y(void) { return g_state.food.position.y - GAME_AREA_Y; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_food_active(void) { return g_state.food.active; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_bonus_x(void) { return g_state.bonus_food.position.x - GAME_AREA_X; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_bonus_y(void) { return g_state.bonus_food.position.y - GAME_AREA_Y; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_bonus_active(void) { return g_state.bonus_food.active; }

EMSCRIPTEN_KEEPALIVE
long wasm_get_bonus_spawn_time(void) { return g_state.bonus_food.spawn_time; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) { return g_state.score; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_high_score(void) { return g_state.high_score; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_running(void) { return g_state.running; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_paused(void) { return g_state.paused; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_tick_count(void) { return g_state.tick_count; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_combo_multiplier(void) { return g_state.combo_multiplier; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_current_level(void) { return g_state.current_level; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_game_mode(void) { return g_state.game_mode; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_frame_delay(void) { return g_state.frame_delay; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_grew(void) { return g_state.grew; }

/* ---- Level Data Getters ---- */

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_wall_count(int lv) { return g_state.levels[lv].wall_count; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_wall_x(int lv, int idx) { return g_state.levels[lv].walls[idx].x - GAME_AREA_X; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_wall_y(int lv, int idx) { return g_state.levels[lv].walls[idx].y - GAME_AREA_Y; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_wrap(int lv) { return g_state.levels[lv].wrap_around; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_target_score(int lv) { return g_state.levels[lv].target_score; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_level_base_speed(int lv) { return g_state.levels[lv].base_speed; }

/* ---- Math library exposed for frontend (proves C code is running) ---- */

EMSCRIPTEN_KEEPALIVE
int wasm_custom_multiply(int a, int b) { return custom_multiply(a, b); }

EMSCRIPTEN_KEEPALIVE
int wasm_custom_divide(int a, int b) { return custom_divide(a, b); }

EMSCRIPTEN_KEEPALIVE
int wasm_custom_modulo(int a, int b) { return custom_modulo(a, b); }

/* ---- Trail data ---- */

EMSCRIPTEN_KEEPALIVE
int wasm_get_trail_count(void) { return g_state.trail_count; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_trail_x(int idx) { return g_state.trail[idx].x - GAME_AREA_X; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_trail_y(int idx) { return g_state.trail[idx].y - GAME_AREA_Y; }
