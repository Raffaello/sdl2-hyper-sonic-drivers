#pragma once

#include <cstdint>
#include <memory>
#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>
#include <HyperSonicDrivers/devices/MT32.hpp>
#include <HyperSonicDrivers/hardware/mt32/MT32.hpp>

namespace HyperSonicDrivers::drivers::midi::mt32
{
    class MT32Driver : public IMidiDriver
    {
    public:
        explicit MT32Driver(const std::shared_ptr<devices::MT32>& mt32);
        ~MT32Driver() override;

        bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) override;
        void close() override;

        //void send(const audio::midi::MIDIEvent& e) noexcept override { /*TODO*/ };
        //void send(const uint32_t msg) noexcept override { /*TODO*/ };
        //void send(const int8_t channel, const uint32_t msg) noexcept override { /*TODO*/ };

        void pause() const noexcept override { /*TODO*/ };
        void resume() const noexcept override { /*TODO*/ };

    protected:
        void onCallback() noexcept override;

        // MIDI events
        void noteOff(const uint8_t chan, const uint8_t note) noexcept override {/*TODO*/};
        void noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept override {/*TODO*/};
        void controller(const uint8_t chan, const audio::midi::MIDI_EVENT_CONTROLLER_TYPES ctrl_type, uint8_t value) noexcept override {/*TODO*/};
        void programChange(const uint8_t chan, const uint8_t program) noexcept override {/*TODO*/};
        void pitchBend(const uint8_t chan, const uint16_t bend) noexcept override {/*TODO*/ };
        void sysEx(const uint8_t* msg, uint16_t length) noexcept override {/*TODO*/ };
        // MIDI Controller Events
        void ctrl_modulationWheel(const uint8_t chan, const uint8_t value) noexcept override {/*TODO*/};
        void ctrl_volume(const uint8_t chan, const uint8_t value) noexcept override {/*TODO*/};
        void ctrl_panPosition(const uint8_t chan, uint8_t value) noexcept override {/*TODO*/};
        void ctrl_sustain(const uint8_t chan, uint8_t value) noexcept override {/*TODO*/ };
        void ctrl_reverb(const uint8_t chan, uint8_t value) noexcept override { /*TODO*/ };
        void ctrl_chorus(const uint8_t chan, uint8_t value) noexcept override { /*TODO*/ };
        void ctrl_allNotesOff() noexcept override {/*TODO*/ };

    private:
        std::shared_ptr<hardware::mt32::MT32> m_mt32;
    };
}
