#include <cstring>
#include <format>
#include <algorithm>
#include <HyperSonicDrivers/drivers/midi/scummvm/AdLibPercussionChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::scummvm
{
    using utils::logD;
    using utils::logW;

    AdLibPercussionChannel::AdLibPercussionChannel() :
        AdLibPart(audio::midi::MIDI_PERCUSSION_CHANNEL)
    {
        priEff = 0;
        volume = 127;

        // Initialize the custom instruments data
        std::ranges::fill(m_notes, 0);
        std::ranges::fill(m_customInstruments, nullptr);
    }

    uint8_t AdLibPercussionChannel::getNote(const uint8_t note) const noexcept
    {
        if (m_customInstruments[note])
            return m_notes[note];
        return note;
    }

    AdLibInstrument* AdLibPercussionChannel::getInstrument(const uint8_t note) const noexcept
    {
        return m_customInstruments[note].get();
    }

    void AdLibPercussionChannel::setCustomInstr(const uint8_t* instr) noexcept
    {
        const uint8_t note = instr[0];
        m_notes[note] = instr[1];

        // Allocate memory for the new instruments
        if (!m_customInstruments[note])
        {
            m_customInstruments[note] = std::make_unique<AdLibInstrument>();
            memset(m_customInstruments[note].get(), 0, sizeof(AdLibInstrument));
        }

        // Save the new instrument data
        m_customInstruments[note]->modCharacteristic = instr[2];
        m_customInstruments[note]->modScalingOutputLevel = instr[3];
        m_customInstruments[note]->modAttackDecay = instr[4];
        m_customInstruments[note]->modSustainRelease = instr[5];
        m_customInstruments[note]->modWaveformSelect = instr[6];
        m_customInstruments[note]->carCharacteristic = instr[7];
        m_customInstruments[note]->carScalingOutputLevel = instr[8];
        m_customInstruments[note]->carAttackDecay = instr[9];
        m_customInstruments[note]->carSustainRelease = instr[10];
        m_customInstruments[note]->carWaveformSelect = instr[11];
        m_customInstruments[note]->feedback = instr[12];
    }
}
