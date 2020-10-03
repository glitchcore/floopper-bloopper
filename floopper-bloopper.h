#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    uint8_t player_x;
    GpioPin* green;
} GameState;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
