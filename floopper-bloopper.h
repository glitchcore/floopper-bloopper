#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    int32_t player_x;
    int32_t player_y;
    int32_t player_vx;
    int32_t player_vy;
    bool player_jump;
    GpioPin* green;
} GameState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BONDARIES_X_LEFT 20
#define BONDARIES_X_RIGHT 108

#define PLAYER_WIDTH 6
#define PLAYER_HEIGHT 6
#define SCALE 1000
#define SPEED_X 50
#define JUMP_SPEED -60

#define FLOOR_HEIGHT 5
