#pragma once

#include <cstdint>

namespace hardware::opl::dosbox
{
    class Timer
    {
    public:
        Timer() = default;
        ~Timer() = default;

        //Call update before making any further changes
        void update(const double time);

        //On a reset make sure the start is in sync with the next cycle
        void reset(const double time);

        void stop();

        void start(const double time, const int scale);

        double startTime = 0.0;
        double delay = 0.0;
        bool enabled = false;
        bool overflow = false;
        bool masked = false;
        uint8_t counter = 0;
    };
}
