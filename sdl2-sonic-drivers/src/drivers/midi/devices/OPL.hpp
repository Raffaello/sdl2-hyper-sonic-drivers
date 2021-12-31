#pragma once

#include <drivers/midi/Device.hpp>
#include <hardware/opl/OPL.hpp>
#include <memory>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            class OPL : public Device
            {
            public:
                explicit OPL(std::shared_ptr<hardware::opl::OPL> opl);
                ~OPL();

                inline void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
                inline void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;

            private:
                std::shared_ptr<hardware::opl::OPL> _opl;
            };
        }
    }
}