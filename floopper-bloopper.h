#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    int32_t player_x;
    int32_t player_y;
    int32_t player_vx;
    int32_t player_yx;
    GpioPin* green;
} GameState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PLAYER_WIDTH 6
#define PLAYER_HEIGHT 6
#define SCALE 1000
#define SPEED_X 50
