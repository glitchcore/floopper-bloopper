#include "flipper.h"
#include "u8g2/u8g2.h"

#include "floopper-bloopper/floopper-bloopper.h"

#include "floopper-bloopper/player_0/player_0_0.xbm"
#include "floopper-bloopper/player_0/player_0_1.xbm"
#include "floopper-bloopper/player_0/player_0_2.xbm"
#include "floopper-bloopper/player_0/player_0_3.xbm"
#include "floopper-bloopper/player_0/player_0_4.xbm"
#include "floopper-bloopper/player_0/player_0_5.xbm"
#include "floopper-bloopper/player_0/player_0_6.xbm"


void render_graphics(GameState* state, u8g2_t* fb) {
    u8g2_ClearBuffer(fb);

    render_ui(state, fb);
    render_world(state, fb);
    render_player(state, fb);
}

void render_player(GameState* state, u8g2_t* fb) {
    if (state->player_x < BONDARIES_X_LEFT * SCALE) {
        state->player_x = BONDARIES_X_LEFT * SCALE;
    } else if (state->player_x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state-> player_x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    }

    unsigned char* player_sprite = NULL;

    if(state->player_vy > 0) {
        player_sprite = player_0_2_bits;
    } else if(state->player_vy < 0) {
        player_sprite = player_0_3_bits;
    } else {
        if(state->player_vx < 0) {
            if(state->player_anim == 0) {
                player_sprite = player_0_0_bits;
            } else {
                player_sprite = player_0_1_bits;
            }
        } else if(state->player_vx > 0) {
            if(state->player_anim == 0) {
                player_sprite = player_0_5_bits;
            } else {
                player_sprite = player_0_6_bits;
            }
        } else {
            player_sprite = player_0_4_bits;
        }
    }

    if(player_sprite != NULL) {
        u8g2_DrawXBM(
            fb,
            state->player_x / SCALE, state->player_y / SCALE,
            player_0_0_width, player_0_0_height,
            (unsigned char*)player_sprite
        );
    }
}

void render_world(GameState* state, u8g2_t* fb) {
    u8g2_SetDrawColor(fb, 1);
    u8g2_DrawBox(fb, 0, SCREEN_HEIGHT - 4, SCREEN_WIDTH, 4);
}

void render_ui(GameState* state, u8g2_t* fb) {
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");

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

    if(input->state) {
        if (input->input == InputRight) {
            state->player_vx = SPEED_X;
        } else if (input->input == InputLeft) {
            state->player_vx = -SPEED_X;
        }
    } else {
        if (input->input == InputRight || input->input == InputLeft) {
            state->player_vx = 0;
        }
    }

    if(input->input == InputUp) {
        if(input->state) {
            state->player_jump = true;
            state->player_vy = JUMP_SPEED;
            state->player_jump = false;
        }
    }

    if(input->input == InputOk) {
        if(input->state) {
            state->combo_panel_activated = !state->combo_panel_activated;
        }
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    update_player_coordinates(state, dt);

    // gravity
    if(state->player_y >= ((SCREEN_HEIGHT - FLOOR_HEIGHT - PLAYER_HEIGHT) * SCALE)) {
        state->player_vy = 0;
    } else {
        state->player_vy += 5;
    }
}

void update_player_coordinates(GameState* state, uint32_t dt) {
    //x
    state->player_x += state->player_vx * dt;

    //x screen
    if (state->player_x < BONDARIES_X_LEFT * SCALE) {
        state->player_x = BONDARIES_X_LEFT * SCALE;
    } else if (state->player_x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state-> player_x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    }

    //y
    state->player_y += state->player_vy * dt;

    state->player_anim = (state->player_x / (SCALE * 4)) % 2;

    //y screen
    if(state->player_y >= ((SCREEN_HEIGHT - FLOOR_HEIGHT - PLAYER_HEIGHT) * SCALE)){
        state->player_y = (SCREEN_HEIGHT - FLOOR_HEIGHT - PLAYER_HEIGHT) * SCALE;
    }
}