
void update_game_state(GameState* state) {
    switch(state->label_id) {
        case WELCOME:
            if(state->player_odo / SCALE > 180 || state->player_odo / SCALE < -160) {
                state->label_id = OMG;
            }
        break;

        default: break;
    }
}