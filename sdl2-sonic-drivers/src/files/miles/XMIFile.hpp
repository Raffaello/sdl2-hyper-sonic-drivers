#pragma once

#include <files/IFFFile.hpp>
#include <audio/midi/MIDIEvent.hpp>
#include <string>
#include <vector>
#include <audio/MIDI.hpp>
#include <memory>
#include <cstdint>

namespace files
{
    namespace miles
    {
        /// <summary>
        /// XMI files play MIDI at a fixed clock rate of 120 Hz.
        /// To play them correctly, the Tempo and PPQN division values
        /// used for playback should therefore also yield a constant 120 Hz,
        /// for example a tempo of 500 000 microseconds and a PPQN of 60,
        /// or a tempo of 1 000 000 microseconds and a PPQN of 120.
        /// Tempo events contained in the EVNT chunks should be ignored;
        /// they are vestigial remnants of the original MIDI file converted
        /// into the XMI format and do not correspond to the values
        /// used by the converted event stream.
        /// </summary>
        class XMIFile final : protected IFFFile
        {
        public:
            XMIFile(const std::string& filename);
            virtual ~XMIFile();

            std::shared_ptr<audio::MIDI> getMIDI() const noexcept;
            //int getNumTracks() const noexcept;
            //const std::vector<uint8_t>& getTrack(const uint16_t track) const noexcept;
        private:
            uint16_t _readFormXdirChunk(IFF_chunk_header_t& form_xdir);
            audio::midi::MIDIEvent _readEvnt(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track);
            void _readTimb(const IFF_sub_chunk_header_t& IFF_timb, const int16_t track);
            void _readRbrn(const IFF_sub_chunk_header_t& IFF_rbrn, const int16_t track);
            
            std::shared_ptr<audio::MIDI> _midi;

            //uint16_t _num_tracks;
            //std::vector<std::vector<uint8_t>> _midi_events;
            
            std::vector<std::vector<uint8_t>> _timbre_patch_numbers;
            std::vector<std::vector<uint8_t>> _timbre_bank;
        };
    }
}
