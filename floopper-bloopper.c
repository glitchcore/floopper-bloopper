#include "flipper.h"
#include "u8g2/u8g2.h"
#include <stdio.h>
#include <math.h>

#include "floopper-bloopper/floopper-bloopper.h"

#include "floopper-bloopper/player_0/player_0_0.xbm"
#include "floopper-bloopper/player_0/player_0_1.xbm"
#include "floopper-bloopper/player_0/player_0_2.xbm"
#include "floopper-bloopper/player_0/player_0_3.xbm"
#include "floopper-bloopper/player_0/player_0_4.xbm"
#include "floopper-bloopper/player_0/player_0_5.xbm"
#include "floopper-bloopper/player_0/player_0_6.xbm"

#define MAX_LINES 4

typedef struct {
    size_t line_size;
    char* lines[MAX_LINES];
} TextBlock;

// narrative
const TextBlock WELCOME = {3,
    {"Welcome to Flopper blooper!", "Use < > to move", "Use ^ to jump"}
};
const TextBlock OMG = {2,
    {"OMG, it's happened again!", "Wait, I try to help you...", "Please, return back"}
};
const TextBlock WRONG = {2,
    {"No, you dawn into cycle deeply", "go back"}
};
const TextBlock STUCK = {2,
    {"Okay, you stuck...", "try to press jump"}
};
const TextBlock HELP_1 = {1, {"...then left"}};
const TextBlock HELP_2 = {1, {"...left again"}};
const TextBlock HELP_3 = {1, {"...now press right"}};
const TextBlock DAMN = {2, {"Damn, it worked before", "I need to read manual"}};

const TextBlock MANUAL = {3,
    {
        "Hey! I found something helpful",
        "You need to activate DCMPA 0x3A77",
        "trigger, do you know what it is?"
    }
};

const TextBlock TIP_0 = {2, {"Maybe some part of", "the earth looks special"}};
const TextBlock TIP_1 = {1, {"Try to jump over it"}};
const TextBlock TIP_HERE = {1, {"Jump here!"}};
const TextBlock TIP_NO_HERE = {1, {"No, not here..."}};

const int32_t HEIGHT_MAP[WORLD_WIDTH] = {
    5000, 5200, 5400, 5600, 5800, 6000, 6200, 6400, 6600, 6800, 7000, 7200, 7400, 7600, 7800, 8000,
    8200, 8400, 8600, 8800, 9000, 9200, 9400, 9600, 9800, 10000, 10200, 10400, 10600, 10800, 11000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 4500, 4000, 3500, 3000, 2500, 2000, 1500, 1000, 500, 0, -500, -1000, -2000, -3000, -4000,
    -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000, -5000,
    -4000, -3000, -2500, -2000, -1500, -500, 0, 500, 1200, 1800, 2500, 3000, 3500, 4000, 4500, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
    5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000,
};

void render_player(GameState* state, u8g2_t* fb);
void render_ui(GameState* state, u8g2_t* fb);
void render_world(GameState* state, u8g2_t* fb);
void handle_player_input(GameState* state, InputEvent* input);
void update_player_coordinates(GameState* state, uint32_t dt);

void render_graphics(GameState* state, u8g2_t* fb) {
    u8g2_ClearBuffer(fb);

    render_ui(state, fb);
    render_world(state, fb);
    render_player(state, fb);
}

void render_player(GameState* state, u8g2_t* fb) {
    if (state->player.x < BONDARIES_X_LEFT * SCALE) {
        state->player.x = BONDARIES_X_LEFT * SCALE;
    } else if (state->player.x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state-> player.x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    }

    unsigned char* player_sprite = NULL;

    if(state->player_v.y > 40) {
        player_sprite = (unsigned char*)player_0_2_bits;
    } else if(state->player_v.y < -40) {
        player_sprite = (unsigned char*)player_0_3_bits;
    } else {
        if(state->player_v.x < 0) {
            if(state->player_anim == 0) {
                player_sprite = (unsigned char*)player_0_0_bits;
            } else {
                player_sprite = (unsigned char*)player_0_1_bits;
            }
        } else if(state->player_v.x > 0) {
            if(state->player_anim == 0) {
                player_sprite = (unsigned char*)player_0_5_bits;
            } else {
                player_sprite = (unsigned char*)player_0_6_bits;
            }
        } else {
            player_sprite = (unsigned char*)player_0_4_bits;
        }
    }

    if(player_sprite != NULL) {
        u8g2_DrawXBM(
            fb,
            state->player.x / SCALE, state->player.y / SCALE,
            player_0_0_width, player_0_0_height,
            (unsigned char*)player_sprite
        );
    }
}

void render_world(GameState* state, u8g2_t* fb) {
    char buf[32];

    // u8g2_SetDrawColor(fb, 1);
    // u8g2_DrawBox(fb, 0, SCREEN_HEIGHT - 4, SCREEN_WIDTH, 4);

    

    u8g2_SetDrawColor(fb, 1);
    for(size_t i = 0; i < SCREEN_WIDTH; i++) {
        int32_t floor_height = HEIGHT_MAP[abs(state->screen.x / SCALE + i) % WORLD_WIDTH];

        // if(floor_height > 0) {
            u8g2_DrawBox(fb,
                i, SCREEN_HEIGHT - (floor_height - state->screen.y) / SCALE,
                1, 5);
        // }
    }

    // in-level label
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    u8g2_DrawStr(fb,
        (LABEL_X - state->screen.x) / SCALE, (LABEL_Y + state->screen.y) / SCALE,
        "Floopper bloopper!"
    );

    /*
    u8g2_DrawBox(fb,
        (5 * SCALE - state->screen.x) / SCALE, (10 * SCALE - state->screen.y) / SCALE,
        10, 10
    );
    */
}

void render_ui(GameState* state, u8g2_t* fb) {
    if(state->combo_panel_activated) {
        u8g2_SetDrawColor(fb, 0);
        u8g2_DrawBox(fb, 0, SCREEN_HEIGHT - 4, SCREEN_WIDTH, 4);
        u8g2_SetDrawColor(fb, 1);
        u8g2_DrawFrame(fb, 10, 20, (SCREEN_WIDTH) - 10 * 2, 20);
    }
}

void handle_key(GameState* state, InputEvent* input) {
    printf(
        "[kb] event: %02x %s\n",
        input->input,
        input->state ? "pressed" : "released"
    );

    if(input->input == InputOk) {
        if(input->state) {
            if(!state->combo_panel_activated) {
                state->combo_panel_cnt = 0;
                state->combo_panel_activated = true;
            } else {
                state->combo_panel_activated = false;
            }
        }
    }

    if(state->combo_panel_activated) {

    } else {
        handle_player_input(state, input);
    }
}

void handle_player_input(GameState* state, InputEvent* input) {
    if(input->state) {
        if (input->input == InputRight) {
            state->player_v.x = SPEED_X;
        } else if (input->input == InputLeft) {
            state->player_v.x = -SPEED_X;
        }
    } else {
        if (input->input == InputRight || input->input == InputLeft) {
            state->player_v.x = 0;
        }
    }

    if(input->input == InputUp) {
        if(input->state) {
            state->player_v.y = JUMP_SPEED;
        }
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    update_player_coordinates(state, dt);

    
}

void update_player_coordinates(GameState* state, uint32_t dt) {
    // apply kinemathic
    state->player.x += state->player_v.x * dt;
    state->player_global.x += state->player_v.x * dt;

    state->player.y += state->player_v.y * dt;
    state->player_global.y += state->player_v.y * dt;

    // gravity + floor

    int32_t floor_height = SCREEN_HEIGHT * SCALE -
        HEIGHT_MAP[(abs(state->player_global.x) / SCALE) % WORLD_WIDTH] -
        PLAYER_HEIGHT * SCALE;

    if(state->player_global.y >= floor_height){
        state->player.y = floor_height;
        state->player_global.y = floor_height;
        state->player_v.y = 0;
    } else {
        state->player_v.y += 5;
    }

    // aply constrains
    if (state->player.x < BONDARIES_X_LEFT * SCALE) {
        state->player.x = BONDARIES_X_LEFT * SCALE;
    } else if (state->player.x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state-> player.x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    }

    if(state->player.y > (SCREEN_HEIGHT - PLAYER_HEIGHT - 4) * SCALE) {
        state->player.y = (SCREEN_HEIGHT - PLAYER_HEIGHT - 4) * SCALE;
    }

    // global
    state->screen.x = state->player_global.x - state->player.x;
    state->screen.y = state->player.y - state->player_global.y;
    
    if (state->player_global.x < BONDARIES_X_LEFT * SCALE) {
        state->player_global.x += WORLD_WIDTH * SCALE;
    }

    state->player_anim = (state->player_global.x / (SCALE * 4)) % 2;
}