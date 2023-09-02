#pragma once

#include <files/File.hpp>
#include <cstdint>
#include <vector>
#include <string>
#include <audio/midi/types.hpp>
#include <audio/MIDI.hpp>
#include <memory>
#include <files/GetMIDI.hpp>

namespace files
{
    class MIDFile : protected File, public GetMIDI
    {
    public:
        MIDFile(const std::string& filename);
        virtual ~MIDFile();

        std::shared_ptr<audio::MIDI> getOriginalMIDI() const noexcept;
        /**
         * @brief if the file is not a single track, it will return a
         *        converted equivalent with single track
         * @return
        */
        std::shared_ptr<audio::MIDI> getMIDI() const override;
    private:
        /// <summary>
        /// Variable length quantity decoding algorithm to read from file
        /// </summary>
        /// <param name="buf">the max 4 bytes array to decode</param>
        /// <param name="out_value">the resulting decoded value</param>
        /// <returns>byte reads</returns>
        int decode_VLQ(uint32_t& out_value);
        
        typedef struct midi_chunk_t
        {
            char id[4];
            uint32_t length;
        } midi_header_t;

        midi_chunk_t read_chunk();
        void read_header();
        void check_format();
        void read_track();

        std::shared_ptr<audio::MIDI> _midi;
    };
}
