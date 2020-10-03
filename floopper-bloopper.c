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

        delay(500);
    }
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

    GpioPin green = {.pin = GPIO_PIN_14, .port = GPIOB};
    // configure pin
    pinMode(green, GpioModeOpenDrain);
    digitalWrite(green, HIGH);

    Event event;

    while(1) {
        if(xQueueReceive(event_queue, (void*)&event, portMAX_DELAY)) {
            digitalWrite(green, LOW);

            u8g2_t* fb = furi_take(fb_record);
            if(fb != NULL) {
                u8g2_SetFont(fb, u8g2_font_6x10_mf);
                u8g2_SetDrawColor(fb, 1);
                u8g2_SetFontMode(fb, 1);
                u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");
            }
            furi_commit(fb_record);

            delay(2);
            digitalWrite(green, HIGH);
        }
    }
}