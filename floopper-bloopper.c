#include "flipper.h"
#include "u8g2/u8g2.h"

#include "floopper-bloopper/floopper-bloopper.h"

void render_graphics(GameState* state, u8g2_t* fb) {
    u8g2_ClearBuffer(fb);
    
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");

    render_player(state, fb);
}

void render_player(GameState* state, u8g2_t* fb) {
    if (state->player_x < BONDARIES_X_LEFT * SCALE) {
        state->player_x = BONDARIES_X_LEFT * SCALE;
    } else if (state->player_x > (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE) {
        state-> player_x = (BONDARIES_X_RIGHT - PLAYER_WIDTH) * SCALE;
    }
    u8g2_DrawBox(fb, state->player_x / SCALE, state->player_y, PLAYER_WIDTH, PLAYER_HEIGHT);
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
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    state->player_x += state->player_vx * dt;
}