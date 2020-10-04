
void update_game_state(GameState* state, uint32_t t, uint32_t dt) {
    switch(state->label_id) {
        case WELCOME:
            state->player_odo += state->player_v.x * dt;

            if(state->player_odo / SCALE > 180 || state->player_odo / SCALE < -160) {
                state->glitch_level = 2;
            }

            if(
                (abs((LABEL_X - state->screen.x) / SCALE)) % 256 < 2 &&
                state->glitch_level == 2 &&
                state->player_t == 0
            ) {
                state->player_t = t;
                state->in_boundaries = true;
            }

            if(state->player_t > 0 && t - state->player_t > 2000) {
                state->player_t = t;

                state->label_id = OMG;
                state->glitch_level = 0;
            }
        break;

        case OMG:
            if(t - state->player_t > 3000) {
                state->label_id = OMG_HELP;
                state->in_boundaries = false;
                state->player_odo = 0;
            }
        break;

        case OMG_HELP:
            state->player_odo += state->player_v.x * dt;

            if(state->player_odo / SCALE > WORLD_WIDTH * 0.8) {
                state->label_id = WRONG;
                state->player_odo = 0;
            }

            if(
                state->player_odo / SCALE < -WORLD_WIDTH * 0.8 &&
                (abs((LABEL_X - state->screen.x) / SCALE)) % 256 < 2
            ) {
                state->label_id = STUCK;
                state->in_boundaries = true;
            }
        break;

        case STUCK:
            if(state->player_v.y < -20) {
                state->label_id = HELP_1;
            }
        break;

        case HELP_1:
            if(state->player_v.x < 0) {
                state->label_id = HELP_2;
            }
        break;

        case HELP_2:
            if(state->player_v.x > 0) {
                state->label_id = HELP_3;
            }
        break;

        case HELP_3:
            if(state->player_v.x < 0) {
                state->label_id = DAMN;
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

    sprintf(buf, "label: %d", (abs((LABEL_X - state->screen.x) / SCALE)) % 256);
    u8g2_DrawStr(fb, 0, 40, buf);
}