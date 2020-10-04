
void update_game_state(GameState* state, uint32_t t, uint32_t dt) {
    switch(state->label_id) {
        case WELCOME:
            state->player_odo += state->player_v.x * dt;

            if(
                (abs((LABEL_X - state->screen.x) / SCALE)) % SCREEN_WIDTH < 10 &&
                (state->player_odo / SCALE > 180 || state->player_odo / SCALE < -160)
            ) {
                state->player_t = t;
                state->glitch_level = 2;
            }

            if(state->player_t > 0 && t - state->player_t > 3000) {
                state->player_t = t;

                state->label_id = OMG;
                state->glitch_level = 0;
            }
        break;

        case OMG:
            if(t - state->player_t > 3000) {
                state->label_id = OMG_HELP;
            }
        break;


        default: break;
    }
}

void render_game_state(GameState* state, u8g2_t* fb) {
    char buf[32];

    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);

    sprintf(buf, "label: %d", abs((LABEL_X - state->screen.x) / SCALE) );
    // u8g2_DrawStr(fb, 0, 40, buf);
}