#include "ScummVM.hpp"
#include <drivers/midi/devices/ScummVM.hpp>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            ScummVM::ScummVM(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode) : Device()
            {
                _adlib = std::make_shared<MidiDriver_ADLIB>(opl, opl3mode);
                _adlib->open();
            }

            ScummVM::~ScummVM()
            {
                _adlib->close();
            }

            inline void ScummVM::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                uint32_t b = e.type.val + (e.data[0]<<8);
                if (e.data.size() == 2)
                    b += (e.data[1] << 16);
                _adlib->send(b);
            }

            inline void ScummVM::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
                assert(size >= 2 && size <= 3);
                uint32_t b = msg[0] + (msg[1] << 8);
                if (size == 3)
                    b += (msg[2] << 16);
                _adlib->send(b);
            }
        }
    }
}
