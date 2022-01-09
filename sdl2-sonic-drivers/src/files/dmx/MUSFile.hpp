#pragma once

#include <files/File.hpp>
#include <audio/MIDI.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace files
{
    namespace dmx
    {
        class MUSFile : protected File
        {
        public:
            static const int MUS_PLAYBACK_SPEED_DEFAULT;
            static const int MUS_PLAYBACK_SPEED_ALTERNATE;

            explicit MUSFile(const std::string& filename, const int playback_speed = MUS_PLAYBACK_SPEED_DEFAULT);
            virtual ~MUSFile() = default;

            std::shared_ptr<audio::MIDI> getMIDI() const noexcept;

            const int playback_speed;
        private:
            /**
             * channels 0-8, 9 is percussion
             * secondary_channels 10-14, 15 percussion
             * percussion channels 9,15 are assigned to MIDI percussion channel 15
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
           
            header_t _header;
            std::vector<uint16_t> instruments; // not used for MIDI

            void readHeader();
            void readTrack();

            std::shared_ptr<audio::MIDI> _midi;
        };
    }
}
