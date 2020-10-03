#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    uint8_t player_x;
    GpioPin* green;
} GameState;
