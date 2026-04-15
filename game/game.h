/* ========================================================================== */
/*                          SNAKE GAME LOGIC                                 */
/*                                                                           */
/*  Core game state, data structures, and game-loop functions for Snake.    */
/*  All rendering uses screen.c, all math uses math.c, etc.                 */
/* ========================================================================== */

#ifndef GAME_H
#define GAME_H

/* ---- Game Area Dimensions ---- */
/* These define the playable region inside the border box.                   */
/* The border itself is drawn at (AREA_X - 1, AREA_Y - 1) and extends      */
/* (AREA_WIDTH + 2) wide by (AREA_HEIGHT + 2) tall.                        */

#define GAME_AREA_X       2    /* Left column of playable area               */
#define GAME_AREA_Y       4    /* Top row of playable area                   */
#define GAME_AREA_WIDTH   60   /* Width of playable area in columns          */
#define GAME_AREA_HEIGHT  20   /* Height of playable area in rows            */

/* Maximum snake body length (should never be reached in Phase 1). */
#define MAX_SNAKE_LENGTH  256

/* ---- Direction Constants ---- */
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* ---- Data Structures ---- */

/* A 2D point on the terminal grid. */
typedef struct
{
    int x;  /* Column */
    int y;  /* Row    */
} Point;

/* The snake: a head + body stored as an array of points.                   */
/* body[0] is always the head.                                               */
typedef struct
{
    Point body[MAX_SNAKE_LENGTH];
    int   length;       /* Current number of body segments (including head)  */
    int   direction;    /* Current movement direction (DIR_UP, etc.)         */
    int   alive;        /* 1 = alive, 0 = dead                              */
} Snake;

/* A food item on the board. */
typedef struct
{
    Point position;     /* Where the food is located                         */
    int   active;       /* 1 = visible on board, 0 = needs respawning       */
    char  symbol;       /* Character displayed for the food (e.g., '*')     */
} Food;

/* Complete game state -- everything needed to describe one frame. */
typedef struct
{
    Snake snake;
    Food  food;
    int   score;        /* Player's current score                            */
    int   running;      /* 1 = game loop active, 0 = exit requested          */
    int   tick_count;   /* Frame counter (used for pseudo-random placement)  */
    Point old_tail;     /* Previous tail position (for erasing during render) */
    int   grew;         /* 1 if snake grew this frame (don't erase tail)    */
} GameState;

/* ---- Game Functions ---- */

/* Initializes the game state: places snake at center, sets up food, etc.   */
void game_init(GameState *state);

/* Processes a single key press and updates the snake's direction.           */
/* Also handles quit (q / ESC).                                              */
void game_process_input(GameState *state, int key);

/* Advances the game by one tick: moves the snake, checks collisions.       */
void game_update(GameState *state);

/* Renders only the parts of the screen that changed (differential render). */
void game_render(GameState *state);

/* Draws the static game border, title, and instructions (called once).     */
void game_render_border(void);

/* Updates the score display on screen. */
void game_render_score(GameState *state);

/* Draws the full snake body (called once at game start). */
void game_render_full_snake(GameState *state);

#endif /* GAME_H */
