#pragma once

#include <cstdint>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            class MidiChannel
            {
            public:
                MidiChannel() = default;
                ~MidiChannel() = default;

                //uint8_t getNumber() override;
                //void release() override;

                //void send(uint32_t b) override;

                // Regular messages
                void noteOff(const uint8_t note) const;
                void noteOn(const uint8_t note, const uint8_t velocity) const;

                //void programChange(uint8_t program) override;
                //void pitchBend(int16_t bend) override;

                // Control Change messages
                //void controlChange(uint8_t control, uint8_t value) override;
                //void modulationWheel(uint8_t value) override;
                //void volume(uint8_t value) override;
                //void panPosition(uint8_t value) override;
                //void pitchBendFactor(uint8_t value) override;
                //void detune(uint8_t value) override;
                //void priority(uint8_t value) override;
                //void sustain(bool value) override;
                //void effectLevel(uint8_t value) override { return; } // Not supported
                //void chorusLevel(uint8_t value) override { return; } // Not supported
                //void allNotesOff() override;
            };
        }
    }
}
