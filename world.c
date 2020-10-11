
void render_world(GameState* state, u8g2_t* fb, uint32_t t) {
    char buf[32];

    u8g2_SetDrawColor(fb, 1);
    for(size_t i = 0; i < SCREEN_WIDTH; i++) {
        int32_t floor_height = HEIGHT_MAP[abs(state->screen.x / SCALE + i) % WORLD_WIDTH];

        u8g2_DrawBox(fb, i, SCREEN_HEIGHT - (floor_height - state->screen.y) / SCALE, 1, 5);
    }

    // in-level label
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);

    if(t - state->glitch_t > 250) {
        state->glitch_t = t;
    }

    const TextBlock* label = &NARRATIVE[state->label_id];
    for(size_t i = 0; i < label->line_size; i++) {
        strcpy(buf, label->lines[i]);

        for(size_t glitch = 0; glitch < state->glitch_level; glitch++) {
            buf[(state->glitch_t + glitch * 23) % strlen(buf)] =
                ' ' + (state->glitch_t + glitch * 17) % ('z' - ' ');
        }

        u8g2_DrawStr(
            fb,
            (LABEL_X - state->screen.x) / SCALE,
            ((LABEL_Y + LABEL_HEIGHT * i) + state->screen.y) / SCALE,
            buf);
    }

    if(state->combo_text) {
        u8g2_SetDrawColor(fb, 0);
        u8g2_DrawBox(fb, SCREEN_WIDTH / 2 - 15, 0, SCREEN_WIDTH / 2 + 15, 25);
        u8g2_SetDrawColor(fb, 1);
        u8g2_DrawFrame(fb, SCREEN_WIDTH / 2 - 15, 0, SCREEN_WIDTH / 2 + 15, 25);
        u8g2_DrawStr(fb, SCREEN_WIDTH / 2 - 10, 15, "COCOCOCOMBO!");
    }
}
