#include <array>
#include <cassert>
#include <HyperSonicDrivers/devices/midi/MidiNative.hpp>

namespace HyperSonicDrivers::devices::midi
{
    MidiNative::MidiNative(const int port) : IMidiDevice()
    {
        // default open port 0
        m_midiOut->openPort(port);
    }

    void MidiNative::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
    {
        std::array<uint8_t, 3> m;
        const size_t size = e.data.size() + 1;
        m[0] = e.type.val;
        m[1] = e.data[0];
        if (size == 3)
            m[2] = e.data[1];

        m_midiOut->sendMessage(m.data(), size);
    }

    void MidiNative::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
    {
        assert(size >= 2 && size <= 3);
        m_midiOut->sendMessage(msg, size);
    }

    void MidiNative::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
    {
        // TODO
    }

    void MidiNative::pause() const noexcept
    {
        // TODO
    }

    void MidiNative::resume() const noexcept
    {
        // TODO
    }
}
