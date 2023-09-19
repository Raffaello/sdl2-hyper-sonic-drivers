#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/devices/IMidiDevice.hpp>
#include <rtmidi/RtMidi.h>


namespace HyperSonicDrivers::devices::midi
{
    /**
     * @brief This is using OS Midi devices
     * It is outside the internal Mixer at the moment.
     * It wasn't neither planned to use it, but it could
     * be useful to have it.
    */
    class MidiNative : public IMidiDevice
    {
    public:
        explicit MidiNative(const int port = 0);
        ~MidiNative() override = default;

        void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override;
        void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override;
        void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override;
        void pause() const noexcept override;
        void resume() const noexcept override;

    private:
        std::unique_ptr<RtMidiOut> m_midiOut = std::make_unique<RtMidiOut>();
    };
}
