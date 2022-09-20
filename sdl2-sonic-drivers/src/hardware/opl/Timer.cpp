#include <hardware/opl/Timer.hpp>
#include <cmath>

namespace hardware::opl
{
    Timer::Timer()
    {
        masked = false;
        overflow = false;
        enabled = false;
        counter = 0;
        delay = 0;

        startTime = 0.0;
    }

    void Timer::update(double time)
    {
        if (!enabled || !delay)
            return;
        double deltaStart = time - startTime;
        // Only set the overflow flag when not masked
        if (deltaStart >= 0 && !masked)
            overflow = 1;
    }

    void Timer::reset(double time)
    {
        overflow = false;
        if (!delay || !enabled)
            return;
        double delta = (time - startTime);
        double rem = fmod(delta, delay);
        double next = delay - rem;
        startTime = time + next;
    }

    void Timer::stop()
    {
        enabled = false;
    }

    void Timer::start(double time, int scale)
    {
        //Don't enable again
        if (enabled)
            return;
        enabled = true;
        delay = 0.001 * (256 - counter) * scale;
        startTime = time + delay;
    }
}
