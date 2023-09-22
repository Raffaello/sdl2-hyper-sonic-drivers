#pragma once

#include <cstdint>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>

namespace HyperSonicDrivers::drivers::midi
{
    /**
    * Interface for all the different Midi drivers
    * so it can be easily used from MIDDrv (maybe rename it to MIDI_player or something)
    **/
    class IMidiDriver
    {
    public:
        IMidiDriver(IMidiDriver&) = delete;
        IMidiDriver(IMidiDriver&&) = delete;
        IMidiDriver& operator=(IMidiDriver&) = delete;

        IMidiDriver() = default;
        virtual ~IMidiDriver() = default;

        virtual bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) = 0;
        virtual void close() = 0;

        inline bool isOpen() const noexcept { return m_isOpen; }

        virtual void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept = 0;
        virtual void send(uint32_t msg) = 0;
        virtual void send(int8_t channel, uint32_t msg) = 0;

        virtual void pause() const noexcept = 0;
        virtual void resume() const noexcept = 0;

    protected:
        bool m_isOpen = false;
    };
}
