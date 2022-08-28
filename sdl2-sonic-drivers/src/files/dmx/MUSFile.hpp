#pragma once

#include <audio/MIDI.hpp>
#include <files/File.hpp>
#include <files/dmx/OP2File.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <array>


namespace files
{
    namespace dmx
    {
        class MUSFile : protected File
        {
        public:
            // TODO: check the MUS_FORM.txt for how to compute ticks speed
            static const int MUS_PLAYBACK_SPEED_DEFAULT;
            static const int MUS_PLAYBACK_SPEED_ALTERNATE;

            explicit MUSFile(const std::string& filename, const int playback_speed = MUS_PLAYBACK_SPEED_DEFAULT);
            ~MUSFile() override = default;

            std::shared_ptr<audio::MIDI> getMIDI(std::shared_ptr<files::dmx::OP2File> op2file) noexcept;
            std::shared_ptr<audio::MIDI> getMIDI() noexcept;

            const int playback_speed;
        private:
            /**
             * channels 0-8, 9 is percussion
             * secondary_channels 10-14, 15 percussion
             * percussion channels 9,15 are assigned to MIDI percussion channel 15 (actually at the moment 9)
             * percurssion is not consider as primary or secondary channel, but just a reserved one
             * 
            */
            typedef struct header_t
            {
                char id[4];
                uint16_t score_len;
                uint16_t score_start;
                uint16_t channels;           // primary channels always to be played
                uint16_t secondary_channels; // secondary channels can be dropped if not supported, for eg in OPL2
                uint16_t instrument_counts;
                uint16_t padding;
            } header_t;
            static_assert(sizeof(header_t) == 16);

            typedef union mus_event_desc_u
            {
                uint8_t val;
                struct e {
                    uint8_t channel : 4;
                    uint8_t type : 3;
                    uint8_t last : 1;
                } e;
            } mus_event_desc_u;
            static_assert(sizeof(mus_event_desc_u) == sizeof(uint8_t));

            typedef struct mus_event_t {
                mus_event_desc_u desc;
                uint32_t delta_time;
                std::vector<uint8_t> data;
            } mus_event_t;

            // Mapping MUS to MIDI Controls
            static const std::array<uint8_t, 15> ctrlMap;

            header_t _header;
            std::vector<uint16_t> instruments; // not used for MIDI

            void readHeader();
            void readTrack();
            std::shared_ptr<audio::MIDI> convertToMidi(std::shared_ptr<files::dmx::OP2File> op2file);

            //std::shared_ptr<audio::MIDI> _midi;
            std::vector<mus_event_t> _mus;
        };
    }
}
