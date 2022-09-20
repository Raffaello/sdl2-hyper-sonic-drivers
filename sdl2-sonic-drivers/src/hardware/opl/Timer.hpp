#pragma once

#include <cstdint>

namespace hardware::opl
{
    struct Timer
    {
        double startTime;
        double delay;
        bool enabled, overflow, masked;
        uint8_t counter;

        Timer();

        //Call update before making any further changes
        void update(double time);

        //On a reset make sure the start is in sync with the next cycle
        void reset(double time);

        void stop();

        void start(double time, int scale);
    };
}
