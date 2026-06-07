#pragma once
#include <3ds.h>
#include "MicrophoneInput.h"
#include <malloc.h>
#include <algorithm>
#include <cmath>
#include <cstring>

class MicrophoneInput
{
private:
    static constexpr u32 BUFFER_SIZE = 0x10000;

    u8 *buffer = nullptr;
    u32 dataSize = 0;
    u32 readOffset = 0;

    float level = 0.0f;
    float noiseLevel = 0.01f;

    bool blowing = false;
    bool blowStarted = false;

public:
    bool init()
    {
        buffer = static_cast<u8 *>(memalign(0x1000, BUFFER_SIZE));

        if (buffer == nullptr)
            return false;

        std::memset(buffer, 0, BUFFER_SIZE);

        if (R_FAILED(micInit(buffer, BUFFER_SIZE)))
        {
            free(buffer);
            buffer = nullptr;
            return false;
        }

        dataSize = micGetSampleDataSize();
        readOffset = 0;

        MICU_SetPower(true);
        MICU_SetGain(40);
        MICU_SetClamp(false);

        Result result = MICU_StartSampling(
            MICU_ENCODING_PCM16_SIGNED,
            MICU_SAMPLE_RATE_16360,
            0,
            dataSize,
            true);

        if (R_FAILED(result))
        {
            MICU_SetPower(false);
            micExit();
            free(buffer);
            buffer = nullptr;
            return false;
        }

        return true;
    }

    void exit()
    {
        if (buffer == nullptr)
            return;

        MICU_StopSampling();
        MICU_SetPower(false);
        micExit();

        free(buffer);
        buffer = nullptr;
    }

    void update()
    {
        if (buffer == nullptr)
            return;

        u32 writeOffset = micGetLastSampleOffset();
        writeOffset &= ~1u;

        if (writeOffset >= dataSize)
            return;

        u64 sum = 0;
        u32 sampleCount = 0;

        while (readOffset != writeOffset)
        {
            s16 sample;
            std::memcpy(&sample, buffer + readOffset, sizeof(sample));

            sum += std::abs(static_cast<int>(sample));
            sampleCount++;

            readOffset += sizeof(sample);

            if (readOffset >= dataSize)
                readOffset = 0;
        }

        if (sampleCount == 0)
            return;

        float rawLevel = static_cast<float>(sum) / static_cast<float>(sampleCount) / 32768.0f;

        level = level * 0.75f + rawLevel * 0.25f;

        if (!blowing)
            noiseLevel = noiseLevel * 0.995f + rawLevel * 0.005f;

        float startThreshold = std::max(0.08f, noiseLevel * 3.0f);
        float stopThreshold = startThreshold * 0.55f;

        if (!blowing && level >= startThreshold)
        {
            blowing = true;
            blowStarted = true;
        }
        else if (blowing && level <= stopThreshold)
        {
            blowing = false;
        }
    }

    bool consumeBlow()
    {
        if (!blowStarted)
            return false;

        blowStarted = false;
        return true;
    }
    float getLevel() const { return level; }
};