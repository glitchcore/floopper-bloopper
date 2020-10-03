#include "flipper.h"
#include "u8g2/u8g2.h"

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
        delay(2);
        digitalWrite(red, HIGH);

        xQueueSend(event_queue, (void*)&tick_event, 0);

        delay(2000);
    }
}

static void event_cb(const void* value, size_t size, void* ctx) {
    QueueHandle_t event_queue = (QueueHandle_t)ctx;

    Event event;
    event.type = EventTypeKey;
    event.value.input = *(InputEvent*)value;
    xQueueSend(event_queue, (void*)&event, 0);
}

typedef struct {
    uint8_t player_x;
} GameState;

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

    GameState state = {.player_x = 50};

    Event event;

    while(1) {
        if(xQueueReceive(event_queue, (void*)&event, portMAX_DELAY)) {
            digitalWrite(green, LOW);

            delay(2);
            digitalWrite(green, HIGH);

            if(event.type == EventTypeKey) {
                handle_key(&state, &event.value.input);
            }

            u8g2_t* fb = furi_take(fb_record);
            if(fb != NULL) {
                render_graphics(&state, fb);
            }
            furi_commit(fb_record);
        }
    }
}