#include <drivers/midi/devices/ScummVM.hpp>
#include <audio/midi/types.hpp>
#include <spdlog/spdlog.h>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            ScummVM::ScummVM(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode) : Device()
            {
                _adlib = std::make_shared<drivers::midi::scummvm::MidiDriver_ADLIB>(opl, opl3mode);
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

            /// <summary>
            /// 
            /// </summary>
            /// <param name="msg">Always 3 size, if it is 2 in size, 3rd value must be zero</param>
            /// <param name="size"></param>
            /// <returns></returns>
            inline void ScummVM::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
                assert(size >= 2 && size <= 3);
                uint32_t b = msg[0] + (msg[1] << 8);
                // the if below should be commented out
                //if (size == 3)
                    b += (msg[2] << 16);
                _adlib->send(b);
            }

            void ScummVM::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // NOTE: TODO: here is replacing an instrument and it should work but it doesnt.
                //             it looks like the OP2 format is not compatible with imuse adlib instr format
                // TODO: Must do a simple MIDI Adlib driver 

                using audio::midi::MIDI_EVENT_TYPES_HIGH;
                using audio::midi::MIDI_META_EVENT_TYPES_LOW;

                if (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high) == MIDI_EVENT_TYPES_HIGH::META_SYSEX &&
                    static_cast<MIDI_META_EVENT_TYPES_LOW>(e.type.low) == MIDI_META_EVENT_TYPES_LOW::SYS_EX0)
                {
                    //if (e.data.size() != (sizeof(uint32_t) + sizeof(OP2File::instrument_t)) {
                    //    // data size not valid ...
                    //}

                    // extract sysEx type
                    uint32_t type = e.data[0] + (e.data[1] << 8) + (e.data[2] << 16) + (e.data[3] << 24);
                    uint8_t channel = e.data[4];
                    _adlib->sysEx_customInstrument(channel, type, &(e.data.data()[5]));
                }
                else
                {
                    // it should not receive a non META SysEx0 message here
                    spdlog::warn("SysEx received a non META SysEx MIDI event");
                    return;
                }
            }
        }
    }
}
