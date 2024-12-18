#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <Arduino.h>
#include <eewl.h>

#include "main.h"
#include "leds.h"

#define ENABLE_SAVEDATA // Use SaveData? SaveData should be setup first and looped last

#define LOOP_INTERVAL_SAVEDATA 232 // how many ms in between loop() ticks for this class?
// note: loop intervals should not be greater than 255 - (DELAY_INTERVAL * 2)

#define DATA_DEFAULT_LED_HUE 213 // default HSV hue (H) for LED colour 
#define DATA_DEFAULT_LED_VALUE 255 // default HSV value (V) for LED colour (brightness)

#define CHECK_INDEX_VALIDITY false

#define FORCE_RESET false // force savedata reset on boot?

#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH 12
#endif

#ifndef BUFFER_START
#define BUFFER_START 0x10
#endif

#ifndef SAVE_INTERVAL
#define SAVE_INTERVAL 1000 // min time in ms between saves
#endif


// container for all local savedata
struct saveData
{
    // have values previously been saved
    bool saved = false;
    byte color = DATA_DEFAULT_LED_HUE;
    byte brightness = DATA_DEFAULT_LED_VALUE;
};

void setupSaveData();
void loopSaveData();
void queueSaveData();
saveData *getSaveData();
void commitSaveData();

bool hasSaved();

#endif