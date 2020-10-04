#include "flipper.h"
#include "u8g2/u8g2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "floopper-bloopper/floopper-bloopper.h"

#define MAX_LINES 4

typedef struct {
    size_t line_size;
    char* lines[MAX_LINES];
} TextBlock;

// narrative
const TextBlock NARRATIVE[] = {
    {3, {"Welcome to Game!", "Use < > to move", "Use ^ to jump"}},
    {3, {"OMG, it's happened", "again! Wait, I try", "to help you..."}},
    {1, {"Please, return back"}},
    {3, {"No, you dawn into", "cycle deeply!", " Go back"}},
    {2, {"Okay, you stuck...", "try to press jump"}},
    {1, {"...then left"}},
    {1, {"...then right"}},
    {1, {"...left again"}},
    {3, {"Damn, it worked", "before. I need to", "read manual"}},
    {2, {"Hey! I found", "something helpful!"}},
    {3, {"You need to activate", "DCMPA 0x3A77 trigger", "u know what it is?"}},
    {3, {"Maybe some part of", "the earth looks", "special"}},
    {1, {"Try to jump over it"}},
    {1, {"Jump here!"}},
    {1, {"No, not here..."}}
};

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

#include "floopper-bloopper/player.c"
#include "floopper-bloopper/world.c"
#include "floopper-bloopper/game.c"

typedef enum {
    ComboInputUp = 0,
    ComboInputDown,
    ComboInputRight,
    ComboInputLeft,
    ComboInputEmpty,
} ComboInput;
ComboInput combo[COMBO_LENGTH];

ComboInput COMBO_PATTERNS[PATTERN_LENGTH][COMBO_LENGTH] = {
        {ComboInputLeft, ComboInputRight, ComboInputRight, ComboInputDown, ComboInputEmpty, ComboInputEmpty, ComboInputEmpty, ComboInputEmpty}, //text combo
        {ComboInputLeft, ComboInputDown, ComboInputRight, ComboInputDown, ComboInputDown, ComboInputEmpty, ComboInputEmpty, ComboInputEmpty}, 
        {ComboInputLeft, ComboInputUp, ComboInputRight, ComboInputDown, ComboInputEmpty, ComboInputEmpty, ComboInputEmpty, ComboInputEmpty}, 
    };

void render_ui(GameState* state, u8g2_t* fb);

void render_graphics(GameState* state, u8g2_t* fb, uint32_t t) {
    u8g2_ClearBuffer(fb);

    render_world(state, fb, t);
    render_player(state, fb);
    render_game_state(state, fb);
    render_ui(state, fb);
}

void render_ui(GameState* state, u8g2_t* fb) {
    if(state->combo_panel_activated) {
        u8g2_SetDrawColor(fb, 0);
        //combo box background
        u8g2_DrawBox(fb, CP_POSITION_X, CP_POSITION_Y, (SCREEN_WIDTH) - CP_POSITION_X * 2, CP_HEIGHT);
        u8g2_SetDrawColor(fb, 1);
        //progress
        u8g2_DrawBox(fb,
            CP_POSITION_X,
            CP_POSITION_Y - CP_PROGRESS_HEIGHT,
            (SCREEN_WIDTH - CP_POSITION_X * 2) * state->combo_progress / ( 100 * SCALE ),
            CP_PROGRESS_HEIGHT);
        //combo box frame
        u8g2_DrawFrame(fb,
            CP_POSITION_X,
            CP_POSITION_Y,
            SCREEN_WIDTH - CP_POSITION_X * 2,
            CP_HEIGHT);

        //combo items
        u8g2_SetFont(fb, u8g2_font_unifont_t_symbols);
        for(size_t i = 0; i < state->combo_panel_cnt; i++) {
            uint16_t item_x = CP_POSITION_X + CP_ITEM_WIDTH + (CP_ITEM_WIDTH + CP_ITEM_SPACE) * i;
            uint16_t item_y = CP_POSITION_Y + (CP_HEIGHT + CP_ITEM_HEIGHT) / 2;
            switch(combo[i]) {
                case ComboInputUp: 
                    u8g2_DrawGlyph(fb, item_x, item_y, 9206);
                break;
                case ComboInputDown: 
                    u8g2_DrawGlyph(fb, item_x, item_y, 9207);
                break;
                case ComboInputRight: 
                    u8g2_DrawGlyph(fb, item_x, item_y, 9205);
                break;
                case ComboInputLeft: 
                    u8g2_DrawGlyph(fb, item_x, item_y, 9204);
                break;
                default: break;
            }
        }
    }
}


void hadle_combo_input(GameState* state, InputEvent* input) {
    if(input->state) {
        combo[state->combo_panel_cnt] = input->input;
        state->combo_progress = 100 * SCALE;
        state->combo_panel_cnt += 1;
        state->combo_speed = ((SCREEN_WIDTH - CP_POSITION_X * 2) * 1000 * state->combo_panel_cnt) / COMBO_TIME;
    }
}

void update_combo_process(GameState* state, uint32_t dt) {
    if(state->combo_panel_activated && (state->combo_progress > 0)) {
        state->combo_progress -= state->combo_speed * dt;
    } else {
        state->combo_panel_activated = false;
        if(!memcmp(COMBO_PATTERNS[0], combo, COMBO_LENGTH)) {
            state->combo_text = true;
        } else if(!memcmp(COMBO_PATTERNS[1], combo, COMBO_LENGTH)) {
            state->combo_text = false;
        }
    }
}

void handle_key(GameState* state, InputEvent* input) {
    printf(
        "[kb] event: %02x %s\n",
        input->input,
        input->state ? "pressed" : "released"
    );

    if(state->combo_panel_activated) {
        hadle_combo_input(state, input);
    } else {
        handle_player_input(state, input);
    }

    if(input->input == InputDown) {
        if(input->state) {
            //for tests
            if(state->in_boundaries){
                state->in_boundaries = false;
            } else {
                state->in_boundaries = true;
            }
        }
    }


    if(input->input == InputOk) {
        if(input->state) {
            if(!state->combo_panel_activated) {
                state->combo_panel_cnt = 0;
                state->combo_panel_activated = true;
                state->combo_progress = 100 * SCALE;
                state->combo_speed = ((SCREEN_WIDTH - CP_POSITION_X * 2) * 1000 * 0.5) / COMBO_TIME;
                for(size_t i = 0; i < COMBO_LENGTH; i++){
                    combo[i] = ComboInputEmpty;
                }
            } else {
                state->combo_panel_activated = false;
            }
        }
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    update_player_coordinates(state, dt);
    update_game_state(state, t, dt);
    update_combo_process(state, dt);
}
