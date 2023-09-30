#pragma once

#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>

namespace HyperSonicDrivers::drivers::midi::mt32
{
    class MT32Driver : public IMidiDriver
    {
    public:
        // TODO

        bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) override { return true; };
        void close() override { /*TODO*/ };

        void send(const audio::midi::MIDIEvent& e) noexcept override { /*TODO*/ };
        void send(const uint32_t msg) noexcept override { /*TODO*/ };
        void send(const int8_t channel, const uint32_t msg) noexcept override { /*TODO*/ };

        void pause() const noexcept override { /*TODO*/ };
        void resume() const noexcept override { /*TODO*/ };

    protected:
        void onCallback() noexcept override { /*TODO*/ };

        // MIDI events
        void noteOff(const uint8_t chan, const uint8_t note) noexcept override {/*TODO*/};
        void noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept override {/*TODO*/};
        void controller(const uint8_t chan, const uint8_t ctrl, uint8_t value) noexcept override {/*TODO*/};
        void programChange(const uint8_t chan, const uint8_t program) noexcept override {/*TODO*/};
        void pitchBend(const uint8_t chan, const uint16_t bend) noexcept override {/*TODO*/ };

        // MIDI Controller Events
        void ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept override {/*TODO*/};
        void ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept override {/*TODO*/};
        void ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept override {/*TODO*/};
        void ctrl_sustain(const uint8_t chan, uint8_t value) const noexcept override {/*TODO*/ };
        void ctrl_allNotesOff() const noexcept override {/*TODO*/ };
    };
}
