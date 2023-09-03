#include <hardware/opl/Timer.hpp>
#include <cmath>

namespace HyperSonicDrivers::hardware::opl
{
    void Timer::update(const double time)
    {
        if (!enabled || !static_cast<bool>(delay))
            return;
        double deltaStart = time - startTime;
        // Only set the overflow flag when not masked
        if (deltaStart >= 0 && !masked)
            overflow = true;
    }

    void Timer::reset(const double time)
    {
        overflow = false;
        if (!static_cast<bool>(delay) || !enabled)
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

    void Timer::start(const double time, const int scale)
    {
        //Don't enable again
        if (enabled)
            return;
        enabled = true;
        delay = 0.001 * (256 - counter) * scale;
        startTime = time + delay;
    }
}
