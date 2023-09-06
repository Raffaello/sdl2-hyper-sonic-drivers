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

    void AdLibPercussionChannel::init(MidiDriver_ADLIB* owner, uint8_t channel)
    {
        AdLibPart::init(owner, channel);
        _priEff = 0;
        _volEff = 127;

        // Initialize the custom instruments data
        std::ranges::fill(_notes, 0);
        std::ranges::fill(_customInstruments, nullptr);
    }

    void AdLibPercussionChannel::noteOff(uint8_t note)
    {
        if (_customInstruments[note])
        {
            note = _notes[note];
        }

        // This used to ignore note off events, since the builtin percussion
        // instrument data has a duration value, which causes the percussion notes
        // to stop automatically. This is not the case for (Groovie's) custom
        // percussion instruments though. Also the OPL3 driver of Sam&Max actually
        // does not handle the duration value, so we need it there too.
        _owner->partKeyOff(this, note);
    }

    void AdLibPercussionChannel::noteOn(uint8_t note, uint8_t velocity) {
        const AdLibInstrument* inst = nullptr;
        const AdLibInstrument* sec = nullptr;

        // The custom instruments have priority over the default mapping
        // We do not support custom instruments in OPL3 mode though.
        if (!_owner->_opl3Mode)
        {
            inst = _customInstruments[note].get();
            if (inst)
                note = _notes[note];
        }

        if (!inst)
        {
            // Use the default GM to FM mapping as a fallback
            uint8_t key = g_gmPercussionInstrumentMap[note];
            if (key != 0xFF) {
                if (!_owner->_opl3Mode)
                {
                    inst = &g_gmPercussionInstruments[key];
                }
                else
                {
                    inst = &g_gmPercussionInstrumentsOPL3[key][0];
                    sec = &g_gmPercussionInstrumentsOPL3[key][1];
                }
            }
        }

        if (!inst)
        {
            logD(std::format("No instrument FM definition for GM percussion key {:d}", note).c_str());
            return;
        }

        _owner->partKeyOn(this, inst, note, velocity, sec, _pan);
    }

    void AdLibPercussionChannel::sysEx_customInstrument(uint32_t type, const uint8_t* instr) {
        // We do not allow custom instruments in OPL3 mode right now.
        if (_owner->_opl3Mode)
        {
            logW("Used in OPL3 mode");
            return;
        }

        if (type == static_cast<uint32_t>('ADLP'))
        {
            uint8_t note = instr[0];
            _notes[note] = instr[1];

            // Allocate memory for the new instruments
            if (!_customInstruments[note])
            {
                _customInstruments[note] = std::make_unique<AdLibInstrument>();
                memset(_customInstruments[note].get(), 0, sizeof(AdLibInstrument));
            }

            // Save the new instrument data
            _customInstruments[note]->modCharacteristic = instr[2];
            _customInstruments[note]->modScalingOutputLevel = instr[3];
            _customInstruments[note]->modAttackDecay = instr[4];
            _customInstruments[note]->modSustainRelease = instr[5];
            _customInstruments[note]->modWaveformSelect = instr[6];
            _customInstruments[note]->carCharacteristic = instr[7];
            _customInstruments[note]->carScalingOutputLevel = instr[8];
            _customInstruments[note]->carAttackDecay = instr[9];
            _customInstruments[note]->carSustainRelease = instr[10];
            _customInstruments[note]->carWaveformSelect = instr[11];
            _customInstruments[note]->feedback = instr[12];
        }
    }
}
