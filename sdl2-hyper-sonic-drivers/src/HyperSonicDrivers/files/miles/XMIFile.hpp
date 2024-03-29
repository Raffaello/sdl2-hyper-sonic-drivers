#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <HyperSonicDrivers/files/IFFFile.hpp>
#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/audio/MIDI.hpp>
#include <HyperSonicDrivers/files/GetMIDI.hpp>

namespace HyperSonicDrivers::files::miles
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
    class XMIFile final : protected IFFFile, public GetMIDI
    {
    public:
        XMIFile(const std::string& filename);
        ~XMIFile() override = default;

        std::shared_ptr<audio::MIDI> getMIDI() const noexcept override;
    private:
        uint16_t _readFormXdirChunk(const IFF_chunk_header_t& form_xdir) const noexcept;
        audio::midi::MIDITrack _readEvnts(const IFF_sub_chunk_header_t& IFF_evnt, const int16_t track) const noexcept;
        void _readTimb(const IFF_sub_chunk_header_t& IFF_timb, const int16_t track);
        void _readRbrn(const IFF_sub_chunk_header_t& IFF_rbrn, const int16_t track);

        std::shared_ptr<audio::MIDI> _midi;
        std::vector<std::vector<uint8_t>> _timbre_patch_numbers;
        std::vector<std::vector<uint8_t>> _timbre_bank;
    };
}
