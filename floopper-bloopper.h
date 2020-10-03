#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    uint8_t player_x;
    uint8_t player_y;
    uint8_t player_vx;
    GpioPin* green;
} GameState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PLAYER_WIDTH 6
#define PLAYER_HEIGHT 6
