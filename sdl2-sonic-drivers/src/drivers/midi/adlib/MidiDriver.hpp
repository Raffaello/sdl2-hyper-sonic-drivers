#pragma once

#include <memory>
#include <cstdint>
//#include <list>

#include <audio/midi/MIDIEvent.hpp>
#include <audio/midi/types.hpp>
#include <audio/opl/banks/OP2Bank.hpp>
#include <drivers/midi/adlib/MidiChannel.hpp>
#include <drivers/midi/adlib/MidiVoice.hpp>
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
            class MidiDriver
            {
            public:
                MidiDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank);
                ~MidiDriver();

                void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept;

            private:
                std::shared_ptr<audio::opl::banks::OP2Bank> _op2Bank;
                std::shared_ptr<hardware::opl::OPL> _opl;
                uint8_t _oplNumChannels = devices::opl::OPL2_NUM_CHANNELS;
                std::array<std::unique_ptr<MidiChannel>, audio::midi::MIDI_MAX_CHANNELS>  _channels;
                std::array<std::unique_ptr<MidiVoice>, devices::opl::OPL2_NUM_CHANNELS> _voices; // TODO shouldn't be connected to MidiChannel instead?
                // TODO this variable is duplicated with OPLWriter... rethink of it..
                std::unique_ptr<devices::opl::OplWriter> _oplWriter;
                uint8_t _playingVoices = 0; // OPL Channels

                void onTimer();

                uint8_t calcVolume(const uint8_t channelVolume, uint8_t noteVolume) const noexcept;

                void releaseSustain(const uint8_t channel);

                uint8_t releaseVoice(const uint8_t slot, const bool killed);

                int allocateVoice(const uint8_t slot, const uint8_t channel, uint8_t note, uint8_t volume, const audio::opl::banks::Op2BankInstrument_t* instrument, const bool secondary, const uint32_t abs_time);

                int8_t findFreeOplChannel(const uint8_t flag,  const uint32_t abs_time);

                /// end "midi" methods

                /// start opl methods

                uint8_t panVolume(const uint8_t volume, int8_t pan) const noexcept;
                /*
                /* Write a Note
                */
                void writeNote(const MidiVoice* voice, const bool keyOn) const noexcept;
            };
        }
    }
}
