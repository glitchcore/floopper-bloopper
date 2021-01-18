#include <furi.h>
#include "floopper-bloopper/floopper-bloopper.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    union {
        InputEvent input;
    } value;
    EventType type;
} AppEvent;

void tick_thread(void* p) {
    osMessageQueueId_t event_queue = p;

    AppEvent tick_event;
    tick_event.type = EventTypeTick;

    // create pin
    // GpioPin red = {.pin = GPIO_PIN_8, .port = GPIOA};

    // configure pin
    // gpio_init(&red, GpioModeOpenDrain);
    // gpio_write(&red, true);

    while(1) {
        // gpio_write(&red, false);
        // gpio_write(&red, true);

        osMessageQueueGet(event_queue, (void*)&tick_event, 0, osWaitForever);

        delay(20);
    }
}

static void event_cb(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = ctx;

    AppEvent event;
    event.type = EventTypeKey;
    event.value.input = *input_event;
    osMessageQueuePut(event_queue, (void*)&event, 0, osWaitForever);
}

void floopper_bloopper(void* p) {
    // create event queue
    osMessageQueueId_t event_queue = osMessageQueueNew(2, sizeof(AppEvent), NULL);

    osThreadAttr_t ticker_attr;
    memset(&ticker_attr, 0x00, sizeof(osThreadAttr_t));
    ticker_attr.name = "tick";
    ticker_attr.stack_size = 1024;
    osThreadNew(tick_thread, (void*)event_queue, &ticker_attr);

    GameState state = {
        .player =
            {
                .x = (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2) * SCALE,
                .y = (SCREEN_HEIGHT / 2) * SCALE,
            },
        .player_global =
            {
                .x = (SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2) * SCALE,
                .y = (SCREEN_HEIGHT / 2) * SCALE,
            },
        .player_v =
            {
                .x = 0,
                .y = 0,
            },

        .in_boundaries = false,
        .player_jump = false,
        .player_anim = 0,

        .combo_panel_activated = false,

        .label_id = WELCOME,

        .glitch_level = 0,
        .glitch_t = 0,

        .player_odo = 0,
        .player_t = 0,

        .combo_text = false,

        .next_level = false,
    };

    state.screen.x = state.player_global.x - state.player.x;
    state.screen.y = state.player_global.y - state.player.y;

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, &state, sizeof(GameState))) {
        printf("[game] cannot create mutex\n");
        furiac_exit(NULL);
    }

    Widget* widget = widget_alloc();

    widget_draw_callback_set(widget, render_graphics, &state_mutex);
    widget_input_callback_set(widget, event_cb, event_queue);

    // Open GUI and register widget
    Gui* gui = (Gui*)furi_open("gui");
    if(gui == NULL) {
        printf("[cc1101] gui is not available\n");
        furiac_exit(NULL);
    }
    gui_add_widget(gui, widget, GuiLayerFullscreen);

    AppEvent event;

    uint32_t t = xTaskGetTickCount();
    uint32_t prev_t = 0;

    while(1) {
        if(osMessageQueueGet(event_queue, (void*)&event, 0, osWaitForever) == osOK) {
            GameState* _state = (GameState*)acquire_mutex_block(&state_mutex);

            if(event.type == EventTypeTick) {
                t = xTaskGetTickCount();
                handle_tick(_state, t, (t - prev_t) % 1024);
                prev_t = t;
            } else if(event.type == EventTypeKey) {
                handle_key(_state, &event.value.input);
            }

            release_mutex(&state_mutex, _state);
            widget_update(widget);
        }
    }
}