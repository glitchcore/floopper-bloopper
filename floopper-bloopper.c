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
    {1, {"...left again"}},
    {1, {"...now press right"}},
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

void render_player(GameState* state, u8g2_t* fb);
void render_ui(GameState* state, u8g2_t* fb);
void render_world(GameState* state, u8g2_t* fb);
void handle_player_input(GameState* state, InputEvent* input);
void update_player_coordinates(GameState* state, uint32_t dt);
void update_game_state(GameState* state);

void render_graphics(GameState* state, u8g2_t* fb) {
    u8g2_ClearBuffer(fb);

    render_ui(state, fb);
    render_world(state, fb);
    render_player(state, fb);
}

void render_world(GameState* state, u8g2_t* fb) {
    char buf[32];

    u8g2_SetDrawColor(fb, 1);
    for(size_t i = 0; i < SCREEN_WIDTH; i++) {
        int32_t floor_height = HEIGHT_MAP[abs(state->screen.x / SCALE + i) % WORLD_WIDTH];

        u8g2_DrawBox(fb,
            i, SCREEN_HEIGHT - (floor_height - state->screen.y) / SCALE,
            1, 5);
    }

    // in-level label
    u8g2_SetFont(fb, u8g2_font_6x10_mf);
    u8g2_SetDrawColor(fb, 1);
    u8g2_SetFontMode(fb, 1);

    sprintf(buf, "odo: %d", state->player_odo);
    u8g2_DrawStr(fb, 0, 40, buf);

    const TextBlock* label = &NARRATIVE[state->label_id];
    for(size_t i = 0; i < label->line_size; i++) {
        strcpy(buf, label->lines[i]);

        u8g2_DrawStr(fb,
            (LABEL_X - state->screen.x) / SCALE,
            ((LABEL_Y + LABEL_HEIGHT * i) + state->screen.y) / SCALE,
            buf
        );
    }
}

void render_ui(GameState* state, u8g2_t* fb) {
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

    if(input->input == InputOk) {
        if(input->state) {
            if(!state->combo_panel_activated) {
                state->combo_panel_cnt = 0;
                state->combo_panel_activated = true;
            } else {
                state->combo_panel_activated = false;
            }
        }
    }

    if(state->combo_panel_activated) {

    } else {
        handle_player_input(state, input);
    }
}

void handle_tick(GameState* state, uint32_t t, uint32_t dt) {
    // printf("t: %d, dt: %d\n", t, dt);

    update_player_coordinates(state, dt);
    update_game_state(state);
}

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
