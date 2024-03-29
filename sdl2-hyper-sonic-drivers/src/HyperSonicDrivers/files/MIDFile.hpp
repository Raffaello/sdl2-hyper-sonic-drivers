#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/files/GetMIDI.hpp>

namespace HyperSonicDrivers::files
{
    class MIDFile : protected File, public GetMIDI
    {
    public:
        MIDFile(const std::string& filename);
        ~MIDFile() override;

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
        int decode_VLQ(uint32_t& out_value) const;
        
        typedef struct midi_chunk_t
        {
            char id[4];
            uint32_t length;
        } midi_header_t;

        midi_chunk_t read_chunk() const noexcept;
        void read_header() noexcept;
        void check_format() const;
        void read_track() const;

        std::shared_ptr<audio::MIDI> _midi;
    };
}
