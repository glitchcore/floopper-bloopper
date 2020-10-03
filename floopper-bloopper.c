#include "flipper.h"
#include "u8g2/u8g2.h"

#include "floopper-bloopper/floopper-bloopper.h"

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

    u8g2_DrawBox(fb, state->player_x / SCALE, state->player_y / SCALE, PLAYER_WIDTH, PLAYER_HEIGHT);
}

void render_world(GameState* state, u8g2_t* fb) {
    u8g2_DrawBox(fb, 0, SCREEN_HEIGHT - 4, SCREEN_WIDTH, 4);
}

void render_ui(GameState* state, u8g2_t* fb) {
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");
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
        }
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    // gravity
    if(state->player_jump) {
        state->player_y -= 1 * SCALE;
        state->player_vy = -60;
        state->player_jump = false;
    } else {
        if(state->player_y > ((SCREEN_HEIGHT - 5 - PLAYER_HEIGHT) * SCALE)) {
            state->player_vy = 0;
        } else {
            state->player_vy += 5;
        }
    }

    state->player_x += state->player_vx * dt;
    state->player_y += state->player_vy * dt;
}