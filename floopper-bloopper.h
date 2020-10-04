#include "flipper.h"
#include "u8g2/u8g2.h"

typedef struct {
    int32_t x;
    int32_t y;
} Vec2;

typedef struct {
    Vec2 player;
    Vec2 player_global;
    Vec2 player_v;
    
    Vec2 screen;

    bool in_boundaries;
    bool player_jump;
    uint8_t player_anim;
    GpioPin* green;

    bool combo_panel_activated;
    uint8_t combo_panel_cnt;
    int32_t combo_progress;
    uint32_t combo_speed;

    size_t label_id;
    uint8_t glitch_level;
    uint32_t glitch_t;

    int32_t player_odo;
    uint32_t player_t;
} GameState;

// global
#define SCALE 1024

// screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BONDARIES_X_LEFT 20
#define BONDARIES_X_RIGHT 108


// player
#define PLAYER_WIDTH 9
#define PLAYER_HEIGHT 12
#define SPEED_X 40
#define JUMP_SPEED -100

// world
#define WORLD_WIDTH 256
#define WORLD_HEIGHT 64

#define LABEL_X 30 * SCALE
#define LABEL_Y 12 * SCALE
#define LABEL_HEIGHT 8 * SCALE

#define TEST_BOX_SIZE 6

enum {
    WELCOME,
    OMG,
    OMG_HELP,
    WRONG,
    STUCK,
    HELP_1,
    HELP_2,
    HELP_3,
    DAMN,
    MANUAL_FOUND,
    MANUAL,
    TIP_0,
    TIP_1,
    TIP_HERE,
    TIP_NO_HERE,
};

//combo
#define CP_POSITION_X 10
#define CP_POSITION_Y 20
#define CP_HEIGHT 20
#define CP_ITEM_WIDTH 8
#define CP_ITEM_HEIGHT 12
#define CP_ITEM_SPACE 5
#define CP_PROGRESS_HEIGHT 3
#define COMBO_TIME 1000
#define COMBO_LENGTH 8
#define PATTERN_LENGTH 3
