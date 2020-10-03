#include "flipper.h"
#include "u8g2/u8g2.h"

void floopper_bloopper(void* p) {
	// TODO try open record and retry on timeout (needs FURI behaviour change)
    delay(1000);

    // open record
    FuriRecordSubscriber* fb_record = furi_open("u8g2_fb", false, false, NULL, NULL, NULL);
    if(fb_record == NULL) {
        printf("[] cannot create fb record\n");
        furiac_exit(NULL);
    }

    // create pin
    GpioPin led = {.pin = GPIO_PIN_8, .port = GPIOA};
    // configure pin
    pinMode(led, GpioModeOpenDrain);
    digitalWrite(led, HIGH);

	while(1) {
		digitalWrite(led, LOW);

		u8g2_t* fb = furi_take(fb_record);
        if(fb != NULL) {
            u8g2_SetFont(fb, u8g2_font_6x10_mf);
            u8g2_SetDrawColor(fb, 1);
            u8g2_SetFontMode(fb, 1);
            u8g2_DrawStr(fb, 2, 12, "Floopper bloopper!");
        }
        furi_commit(fb_record);

        delay(1);
        digitalWrite(led, HIGH);

        delay(1000);
	}
}