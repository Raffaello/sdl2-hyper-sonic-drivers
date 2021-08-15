#include <hardware/opl/OPL.hpp>
#include <spdlog/spdlog.h>

namespace hardware
{
    namespace opl
    {
        OPL::OPL()
        {
            if (_hasInstance) {
                spdlog::error("There are multiple OPL output instances running");
            }

            _hasInstance = true;
        }

        OPL::~OPL()
        {
            _hasInstance = false;
        }

        void OPL::start(TimerCallBack* callback, int timerFrequency)
        {
            _callback.reset(callback);
            startCallbacks(timerFrequency);
        }

        void OPL::stop()
        {
            stopCallbacks();
            _callback.reset();
        }
    }
}
