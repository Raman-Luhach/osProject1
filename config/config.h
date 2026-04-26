#ifndef CONFIG_H
#define CONFIG_H

/* ---- Game Area ---- */
#define GAME_AREA_X       2
#define GAME_AREA_Y       4
#define GAME_AREA_WIDTH   60
#define GAME_AREA_HEIGHT  20

/* ---- Snake ---- */
#define MAX_SNAKE_LENGTH  256
#define INITIAL_SNAKE_LENGTH 3

/* ---- Obstacles ---- */
#define MAX_OBSTACLES     64
#define MAX_LEVELS        5

/* ---- Trail ---- */
#define TRAIL_SIZE        3

/* ---- Scoring ---- */
#define SCORE_NORMAL_FOOD   10
#define SCORE_BONUS_FOOD    50
#define MAX_COMBO_MULTIPLIER 5

/* ---- Timing (microseconds) ---- */
#define BASE_FRAME_DELAY    100000
#define MIN_FRAME_DELAY     40000
#define SPEED_INCREASE_PER_50_POINTS 5000
#define VERTICAL_SPEED_FACTOR_NUM   14
#define VERTICAL_SPEED_FACTOR_DEN   10

/* ---- Combo ---- */
#define COMBO_WINDOW_MS     3000

/* ---- Bonus Food ---- */
#define BONUS_DURATION_MS   5000
#define BONUS_SPAWN_INTERVAL 100

/* ---- Sound Files ---- */
#define SOUND_EAT       "sounds/eat.wav"
#define SOUND_BONUS     "sounds/bonus.wav"
#define SOUND_DIE       "sounds/die.wav"
#define SOUND_LEVEL     "sounds/level.wav"

/* ---- High Score ---- */
#define HIGH_SCORE_FILE ".vasuki_highscore"

/* ---- Level Target Scores ---- */
#define LEVEL_1_TARGET  30
#define LEVEL_2_TARGET  80
#define LEVEL_3_TARGET  150
#define LEVEL_4_TARGET  240
#define LEVEL_5_TARGET  350

/* ---- Level Speeds (microseconds) ---- */
#define LEVEL_1_SPEED   100000
#define LEVEL_2_SPEED   90000
#define LEVEL_3_SPEED   80000
#define LEVEL_4_SPEED   70000
#define LEVEL_5_SPEED   60000

#endif
