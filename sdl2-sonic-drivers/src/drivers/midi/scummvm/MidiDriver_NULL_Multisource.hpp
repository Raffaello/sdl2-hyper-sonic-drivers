#pragma once

#include <cstdint>
#include <drivers/midi/scummvm/MidiDriver_Multisource.hpp>
#include <drivers/midi/scummvm/MidiChannel.hpp>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            class MidiDriver_NULL_Multisource : public MidiDriver_Multisource {
            public:
                ~MidiDriver_NULL_Multisource();

                int open() override;
                bool isOpen() const override { return true; }
                void close() override { }
                uint32_t getBaseTempo() override { return 10000; }
                MidiChannel* allocateChannel() override { return 0; }
                MidiChannel* getPercussionChannel() override { return 0; }

                using MidiDriver_Multisource::send;
                void send(int8_t source, uint32_t b) override { }
                using MidiDriver_Multisource::stopAllNotes;
                void stopAllNotes(uint8_t source, uint8_t channel) override { }

                static void timerCallback(void* data);

            protected:
                void applySourceVolume(uint8_t source) override { }
            };
        }
    }
}
