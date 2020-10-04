#include "flipper.h"
#include "u8g2/u8g2.h"
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
} Event;

void tick_thread(void* p) {
    QueueHandle_t event_queue = (QueueHandle_t)p;

    Event tick_event;
    tick_event.type = EventTypeTick;

    // create pin
    GpioPin red = {.pin = GPIO_PIN_8, .port = GPIOA};
    
    // configure pin
    pinMode(red, GpioModeOpenDrain);
    digitalWrite(red, HIGH);

    while(1){
        digitalWrite(red, LOW);
        digitalWrite(red, HIGH);

        xQueueSend(event_queue, (void*)&tick_event, 0);

        delay(20);
    }
}

static void event_cb(const void* value, size_t size, void* ctx) {
    QueueHandle_t event_queue = (QueueHandle_t)ctx;

    Event event;
    event.type = EventTypeKey;
    event.value.input = *(InputEvent*)value;
    xQueueSend(event_queue, (void*)&event, 0);
}

void floopper_bloopper(void* p) {
    // create event queue
    QueueHandle_t event_queue = xQueueCreate(2, sizeof(Event));

    furiac_start(tick_thread, "tick", (void*)event_queue);

    // TODO try open record and retry on timeout (needs FURI behaviour change)
    delay(1000);

    // open record
    FuriRecordSubscriber* fb_record = furi_open("u8g2_fb", false, false, NULL, NULL, NULL);
    if(fb_record == NULL) {
        printf("[] cannot create fb record\n");
        furiac_exit(NULL);
    }

    furi_open("input_events", false, false, event_cb, NULL, event_queue);

    GpioPin green = {.pin = GPIO_PIN_14, .port = GPIOB};
    // configure pin
    pinMode(green, GpioModeOpenDrain);
    digitalWrite(green, HIGH);

    GameState state = {
        .player = {
            .x = (SCREEN_WIDTH/2 - PLAYER_WIDTH/2) * SCALE,
            .y = (SCREEN_HEIGHT/2) * SCALE,
        },
        .player_global = {
            .x = (SCREEN_WIDTH/2 - PLAYER_WIDTH/2) * SCALE,
            .y = (SCREEN_HEIGHT/2) * SCALE,
        },
        .player_v = {.x = 0, .y = 0,},

        .player_jump = false,
        .player_anim = 0,
        .green = &green,

        .combo_panel_activated = false,

        .label_id = WELCOME,

        .glitch_level = 0,

        .player_odo = 0,
    };

    state.screen.x = state.player_global.x - state.player.x;
    state.screen.y = state.player_global.y - state.player.y;

    Event event;

    uint32_t t = xTaskGetTickCount();
    uint32_t prev_t = 0;

    while(1) {
        if(xQueueReceive(event_queue, (void*)&event, portMAX_DELAY)) {
            digitalWrite(green, LOW);

        	t = xTaskGetTickCount();

            if(event.type == EventTypeTick) {
                handle_tick(&state, t, (t - prev_t) % 1024);
            }

            prev_t = t;

            if(event.type == EventTypeKey) {
                handle_key(&state, &event.value.input);
            }

            u8g2_t* fb = furi_take(fb_record);
            if(fb != NULL) {
                render_graphics(&state, fb);
            }
            furi_commit(fb_record);

            digitalWrite(green, HIGH);
        }
    }
}