#include "flipper.h"
#include "u8g2/u8g2.h"

#include "floopper-bloopper/floopper-bloopper.h"

void render_graphics(GameState* state, u8g2_t* fb) {
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
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    digitalWrite(*state->green, LOW);
    delay(2);
    digitalWrite(*state->green, HIGH);
}