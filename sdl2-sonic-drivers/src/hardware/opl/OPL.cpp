#include <hardware/opl/OPL.hpp>
#include <SDL2/SDL_log.h>
#include <format>

namespace hardware
{
    namespace opl
    {
        // TODO: review to allow to have multiple OPL chips instead.
        static bool _hasInstance;

        OPL::OPL(const OplType type) : type(type)
        {
            if (_hasInstance) {
                SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "There are multiple OPL output instances running");
            }

            _hasInstance = true;
        }

        OPL::~OPL()
        {
            _hasInstance = false;
        }

        void OPL::start(const std::shared_ptr<TimerCallBack>& callback, int timerFrequency)
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
