#include "savedata.h"

#include "leds.h"

static byte _loopIntervalSaveData = 0; // timer to keep track of loop() intervals for this class

saveData data;

EEWL eewlData(data, BUFFER_LENGTH, BUFFER_START);

#ifdef ENABLE_SAVEDATA
bool queuedSave = false;
static int saveDelay = 0;
#endif

void setupSaveData()
{
#ifndef ENABLE_SAVEDATA
    // do not initialize save data
    // (technically redundant since setupSaveData never gets called,
    //  but I want to ensure the compiler doesn't bother with EEWL stuff)
    return;
#else
    // begin EEWL object
    eewlData.begin();

    if (FORCE_RESET)
    {
        eewlData.fastFormat();
    }

    // initial data get
    if (eewlData.get(data))
    {
        // data found
    }
    else
    {
        // data not found, first run
        data.saved = false;
    }
#endif
}

void loopSaveData()
{
#ifdef ENABLE_SAVEDATA
    // check savedata loop delay
#if defined(LOOP_INTERVAL_SAVEDATA) && LOOP_INTERVAL_SAVEDATA > 1
    _loopIntervalSaveData += DELAY_INTERVAL;
    if (_loopIntervalSaveData >= LOOP_INTERVAL_SAVEDATA)
    {
        _loopIntervalSaveData -= LOOP_INTERVAL_SAVEDATA;
    }
    else
    {
        return;
    }
#endif
    // decrement save delay
    if (saveDelay > 0)
    {
        saveDelay -= LOOP_INTERVAL_SAVEDATA;
        if (saveDelay < 0)
        {
            saveDelay = 0;
        }
        return;
    }

    // apply queued save state
    if (queuedSave)
    {
        commitSaveData();
        queuedSave = false;
        saveDelay = SAVE_INTERVAL;
    }
#endif
}

void queueSaveData()
{
#ifdef ENABLE_SAVEDATA
    queuedSave = true;
#endif
}

saveData *getSaveData()
{
#ifndef ENABLE_SAVEDATA
    // if savedata is disabled, force set "saved" flag to true, to allow functionality w/o save
    data.saved = true;
#endif
    return &data;
}

void commitSaveData()
{
#ifndef ENABLE_SAVEDATA
    // can't write savedata if system is disabled (eewlData is uninitialized)
    return;
#else
    data.saved = true;
    eewlData.put(data);
#endif
}

bool hasSaved()
{
#ifndef ENABLE_SAVEDATA
    // if savedata is disabled, "hasSaved" checks should always return true
    return true;
#else
    return data.saved;
#endif
}