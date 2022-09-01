#pragma once

#include <memory>
#include <cstdint>
//#include <list>

#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/types.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/adlib/OplChannel.hpp>
#include <drivers/midi/adlib/OplVoice.hpp>
#include <drivers/midi/devices/opl/OplWriter.hpp>
#include <hardware/opl/OPL.hpp>
#include <hardware/opl/OPL2instrument.h>


namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            /// <summary>
            /// OPL2 MidiDriver.
            /// TODO: OPL3 later
            /// 
            /// Requires instruments as input.
            /// OP2File based data bank. (It is specific to this file how to play notes and take out instruments)
            /// TODO: generalize later on...
            /// </summary>
            class AdLibDriver
            {
            public:
                AdLibDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~AdLibDriver();

                void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept;

            private:
                std::shared_ptr<hardware::opl::OPL> _opl;
                uint8_t _oplNumChannels = devices::opl::OPL2_NUM_CHANNELS;
                std::array<std::unique_ptr<OplChannel>, audio::midi::MIDI_MAX_CHANNELS>  _channels;
                std::array<std::unique_ptr<OplVoice>, devices::opl::OPL2_NUM_CHANNELS> _voices;

                //std::list< uint8_t> _queueChannelVoice;


                std::unique_ptr<devices::opl::OplWriter> _oplWriter;
                uint8_t _playingVoices = 0; // OPL Channels

                // MIDI Events
                void noteOff(const uint8_t chan, const uint8_t note) noexcept;
                void noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol, const uint32_t abs_time) noexcept;
                void controller(const uint8_t chan, const uint8_t ctrl, uint8_t value, const uint32_t abs_time) noexcept;
                void programChange(const uint8_t chan, const uint8_t program) const noexcept;
                void pitchBend(const uint8_t chan, const uint16_t bend, const uint32_t abs_time) const noexcept;

                // MIDI Controller Events
                void ctrl_modulationWheel(const uint8_t chan, const uint8_t value, const uint32_t abs_time) const noexcept;
                void ctrl_volume(const uint8_t chan, const uint8_t value, const uint32_t abs_time) const noexcept;
                void ctrl_panPosition(const uint8_t chan, uint8_t value, const uint32_t abs_time) const noexcept;
                void ctrl_sustain(const uint8_t chan, uint8_t value) noexcept;

                void onTimer();

                void releaseSustain(const uint8_t channel);
                uint8_t releaseVoice(const uint8_t slot, const bool killed);
                int allocateVoice(const uint8_t slot, const uint8_t channel,
                    const uint8_t note_, const uint8_t volume,
                    const audio::opl::banks::Op2BankInstrument_t* instrument,
                    const bool secondary, const uint32_t abs_time);
                
                int8_t findFreeOplVoiceIndex(const uint8_t flag,  const uint32_t abs_time);
            };
        }
    }
}
