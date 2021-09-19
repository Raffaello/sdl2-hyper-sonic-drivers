#pragma once

#include <files/IFFFile.hpp>
#include <string>
#include <vector>
#include <audio/MIDI.hpp>
#include <memory>
#include <cstdint>

namespace files
{
    namespace miles
    {
        class XMIFile final : protected IFFFile
        {
        public:
            XMIFile(const std::string& filename);
            virtual ~XMIFile();

            //int getNumTracks() const noexcept;
            //const std::vector<uint8_t>& getTrack(const uint16_t track) const noexcept;
        private:
            void _readFormXdirChunk();
            void _readEvnt(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track);
            void _readTimb(const IFF_sub_chunk_header_t& IFF_timb, const int16_t track);
            void _readRbrn(const IFF_sub_chunk_header_t& IFF_rbrn, const int16_t track);
            
            std::shared_ptr<audio::MIDI> _midi;

            uint16_t _num_tracks;
            std::vector<std::vector<uint8_t>> _midi_events;
            std::vector<std::vector<uint8_t>> _timbre_patch_numbers;
            std::vector<std::vector<uint8_t>> _timbre_bank;
        };
    }
}
