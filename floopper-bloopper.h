#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    int32_t player_x;
    int32_t player_y;
    int32_t player_global_x;
    int32_t player_global_y;

    int32_t player_vx;
    int32_t player_vy;

    int32_t screen_x;
    int32_t screen_y;

    bool player_jump;
    uint8_t player_anim;
    GpioPin* green;
} GameState;

//global
#define SCALE 1000

//screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BONDARIES_X_LEFT 20
#define BONDARIES_X_RIGHT 108


//player
#define PLAYER_WIDTH 9
#define PLAYER_HEIGHT 12
#define SPEED_X 40
#define JUMP_SPEED -60

//world
#define WORLD_WIDTH 1000
#define WORLD_HIGHT 64
#define FLOOR_HEIGHT 5

#define LABEL_X 2
#define LABEL_Y 12 
