#include "flipper.h"
#include "u8g2/u8g2.h"

#include "floopper-bloopper/floopper-bloopper.h"

void render_graphics(GameState* state, u8g2_t* fb) {
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);
    u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");
    render_player(state, fb);
}

void render_player(GameState* state, u8g2_t* fb) {
    u8g2_DrawBox(fb, state->player_x, state->player_y, PLAYER_WIDTH, PLAYER_HEIGHT);
}

void handle_key(GameState* state, InputEvent* input) {
    printf(
        "[kb] event: %02x %s\n",
        input->input,
        input->state ? "pressed" : "released"
    );

    if (input->input == InputRight) {
        if (input->state) {
            state->player_x += 3;
        }
    } else if (input->input == InputLeft) {
        if (input->state) {
            state->player_x -= 3;
        }
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    printf("t: %d, dt: %d\n", t, dt);

    digitalWrite(*state->green, LOW);
    delay(2);
    digitalWrite(*state->green, HIGH);
}