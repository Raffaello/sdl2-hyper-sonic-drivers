#include <hardware/opl/OPL.hpp>
#include <spdlog/spdlog.h>

namespace hardware
{
    namespace opl
    {
        // TODO: review to allow to have multiple OPL chips instead.
        static bool _hasInstance;

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

        void OPL::start(std::shared_ptr<TimerCallBack> callback, int timerFrequency)
        {
            _callback = callback;
            startCallbacks(timerFrequency);
        }

        void OPL::stop()
        {
            stopCallbacks();
            _callback.reset();
        }
    }
}
