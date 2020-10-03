#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    uint8_t player_x;
    GpioPin* green;
} GameState;

const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;
