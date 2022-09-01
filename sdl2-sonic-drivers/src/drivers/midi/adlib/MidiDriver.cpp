#include <drivers/midi/adlib/MidiDriver.hpp>
#include <spdlog/spdlog.h>
#include<utils/algorithms.hpp>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            using audio::midi::MIDI_PERCUSSION_CHANNEL;
            using audio::midi::MIDI_EVENT_TYPES_HIGH;
            using hardware::opl::OPL2instrument_t;
            using utils::getMillis;

            // this could be in the OP2 Bank probably...
            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH; 
            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE;

            using namespace devices::opl; // TODO remove

            constexpr int SUSTAIN_THRESHOLD = 64;
            constexpr int VIBRATO_THRESHOLD = 40;   /* vibrato threshold */

            constexpr int8_t HIGHEST_NOTE = 127;


            // TODO: clean up and refactor the class
            // TODO: split up in MidiChannel and MidiVoice etc.. (AdlibChannel, AdlibVoice ??)

            // TODO: when no channel is allocated having a for loop to search for nothing is silly.
            
            // !!!!!!!!!!!!!!!!!!!!!!!!!
            // TODO: would make sense to use a doubly linked list as the oldest, front, will be removed
            //       as a if it was like a Queue for channels
            //       otherwise a Set to search the oldest, better than an arary
            // !!!!!!!!!!!!!!!!!!!!!!!!!


            /// TODO: this whole can just become the device::AdLib ....

            // TODO: Adlib is mono so the PAN message/event/command can be skipped in OPL2
            // TODO: secondary channel in OPL2/AdLib won't be used so can be removed.

            MidiDriver::MidiDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) :
                _opl(opl), _op2Bank(op2Bank)
            {
                // TODO: force to be adlib now
                _oplWriter = std::make_unique<OplWriter>(_opl, false);

                if(!_oplWriter->init())
                    spdlog::error("[MidiDriver] Can't initialize AdLib Emulator OPL chip.'");

                for (int i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i) {
                    _channels[i] = std::make_unique<MidiChannel>(i == MIDI_PERCUSSION_CHANNEL, _op2Bank);
                }

                for (int i = 0; i < _oplNumChannels; ++i) {
                    _voices[i] = std::make_unique<MidiVoice>(i);
                }

                hardware::opl::TimerCallBack cb = std::bind(&MidiDriver::onTimer, this);
                auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
                _opl->start(p);
            }

            MidiDriver::~MidiDriver()
            {
                // deinit
                _oplWriter.reset();
                _opl->stop();
            }

            void MidiDriver::onTimer()
            {
            }

            void MidiDriver::send(const audio::midi::MIDIEvent& e) noexcept
            {
                uint32_t abs_time = getMillis<uint32_t>();

                switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
                {
                case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
                {
                    uint8_t chan = e.type.low;
                    uint8_t note = e.data[0];
                    uint8_t sustain = _channels[chan]->_sustain;

                    for (int i = 0; i < _oplNumChannels; i++)
                    {
                        MidiVoice* voice = _voices[i].get();
                        if (voice->note == note && voice->channel == chan)
                        {
                            if (sustain < SUSTAIN_THRESHOLD)
                                releaseVoice(i, 0);
                            else
                                voice->sustain = true;
                        }
                    }

                    spdlog::debug("noteOff {} {} ({})", chan, note, _playingVoices);
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
                {
                    uint8_t chan = e.type.low;
                    uint8_t note = e.data[0];
                    uint8_t volume = e.data[1];
                    int8_t freeSlot = 0;
                    
                    if ((freeSlot = findFreeOplChannel((chan == MIDI_PERCUSSION_CHANNEL) ? 2 : 0, abs_time)) != -1)
                    {
                        // todo make 1 line from these 2 below.
                        auto instr = _channels[chan]->setInstrument(note);
                        int chi = allocateVoice(freeSlot, chan, note, volume, instr, false, abs_time);

                        // TODO: OPL3
                        //if (!OPLsinglevoice && instr->flags == FL_DOUBLE_VOICE)
                        //{
                        //    if ((i = findFreeChannel(mus, (channel == PERCUSSION) ? 3 : 1)) != -1)
                        //        occupyChannel(mus, i, channel, note, volume, instr, 1);
                        //}

                        MidiVoice* voice = _voices[chi].get();
                        spdlog::debug("noteOn note={:d} ({:d}) - vol={:d} ({:d}) - pitch={:d} - ch={:d}", voice->note, voice->realnote, voice->volume, voice->realvolume, voice->pitch, voice->channel);
                    }
                    else {
                        spdlog::critical("NO FREE CHANNEL? midi-ch={} - playingChannels={}", chan, _playingVoices);
                        for (int i = 0; i < _oplNumChannels; i++) {
                            MidiVoice* voice = _voices[i].get();
                            spdlog::critical("OPL channels: {} - free? {}", i, voice->free);
                        }
                    }
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
                    spdlog::warn("AFTERTOUCH not supported");
                    break;
                case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
                {
                    uint8_t chan = e.type.low;
                    uint8_t control = e.data[0];
                    uint8_t value = e.data[1];
                    // MIDI_EVENT_CONTROLLER_TYPES
                    switch (control)
                    {
                    case 0:
                    case 32:
                        // Bank select. Not supported
                        spdlog::warn("bank select value {}", value);
                        break;
                    case 1:
                    {
                        //modulationWheel(value);
                        _channels[chan]->_modulation = value;
                        for (int i = 0; i < _oplNumChannels; i++)
                        {
                            MidiVoice* voice = _voices[i].get();
                            if (voice->channel == chan && (!voice->free))
                            {
                                bool vibrato = voice->vibrato;
                                voice->time = abs_time;
                                if (value >= VIBRATO_THRESHOLD)
                                {
                                    if (!voice->vibrato)
                                        _oplWriter->writeModulation(i, voice->instr, 1);
                                    voice->vibrato = true;

                                }
                                else {
                                    if (voice->vibrato)
                                        _oplWriter->writeModulation(i, voice->instr, 0);
                                    voice->vibrato = false;

                                }
                            }
                        }
                    }
                        spdlog::debug("modwheel value {}", value);
                        break;
                    case 7:
                        {
                        // Volume
                            int i;
                            _channels[chan]->_volume = value;
                            for (i = 0; i < _oplNumChannels; i++)
                            {
                                MidiVoice* ch = _voices[i].get();
                                if (ch->channel == chan && (!ch->free))
                                {
                                    ch->time = abs_time;
                                    ch->realvolume = calcVolume(value, ch->volume);
                                    _oplWriter->writeVolume(i, ch->instr, ch->realvolume);
                                }
                            }
                            spdlog::debug("volume value {} -ch={}", value, i);

                        }
                        break;
                    case 10:
                        // TODO: pan not available in Adlib/OPL2, can be removed/skipped
                        //panPosition(value);
                        {
                        _channels[chan]->_pan = value -= 64;
                            for (int i = 0; i < _oplNumChannels; i++)
                            {
                                MidiVoice* ch = _voices[i].get();
                                if (ch->channel == chan && (!ch->free))
                                {
                                    ch->time = abs_time;
                                    _oplWriter->writePan(i, ch->instr, value);
                                }
                            }
                        }
                        spdlog::debug("panPosition value {}", value);
                        break;
                    case 16:
                        //pitchBendFactor(value);
                        spdlog::warn("pitchBendFactor value {}", value);
                        break;
                    case 17:
                        //detune(value);
                        spdlog::warn("detune value {}", value);
                        break;
                    case 18:
                        //priority(value);
                        spdlog::warn("priority value {}", value);
                        break;
                    case 64:
                        //sustain(value > 0);
                        spdlog::warn("sustain value {}", value);
                        {
                            _channels[chan]->_sustain = value;
                            if (value < SUSTAIN_THRESHOLD) {
                                releaseSustain(chan);
                            }
                        }
                        break;
                    case 91:
                        // Effects level. Not supported.
                        //effectLevel(value);
                        spdlog::warn("effect level value {}", value);
                        break;
                    case 93:
                        // Chorus level. Not supported.
                        //chorusLevel(value);
                        spdlog::warn("chorus level value {}", value);
                        break;
                    case 119:
                        // Unknown, used in Simon the Sorcerer 2
                        spdlog::warn("unkwon value {}", value);
                        break;
                    case 121:
                        // reset all controllers
                        spdlog::warn("reset all controllers value");
                        //modulationWheel(0);
                        //pitchBendFactor(0);
                        //detune(0);
                        //sustain(false);
                        break;
                    case 123:
                        spdlog::debug("all notes off");
                        _oplWriter->stopAll();
                        break;
                    default:
                        spdlog::warn("AdLib: Unknown control change message {:d} {:d}", control, value);
                    }
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
                {
                    uint8_t chan = e.type.low;
                    uint8_t program = e.data[0];

                    _channels[chan]->programChange(program);

                    spdlog::debug("program change {} {} ({})", chan, program, _op2Bank->getInstrumentName(program));
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                    spdlog::warn("CHANNEL_AFTERTOUCH not supported");
                    break;
                case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                {
                    uint8_t chan = e.type.low;
                    uint16_t bend = (e.data[0] | (e.data[1] << 7) - 0x2000) >> 6;
                    spdlog::debug("PITCH_BEND {}", bend);

                    // OPLPitchWheel
                    _channels[chan]->_pitch = static_cast<int8_t>(bend);
                    for (int i = 0; i < _oplNumChannels; i++)
                    {
                        //channelEntry* ch = &_oplChannels[i];
                        MidiVoice* voice = _voices[i].get();
                        if (voice->channel == chan && (!voice->free))
                        {
                            voice->time = abs_time;
                            voice->pitch = voice->finetune + bend;
                            writeNote(voice, true);
                        }
                    }
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
                    spdlog::warn("META_SYSEX not supported");
                    break;

                default:
                    spdlog::warn("MidiDriver: Unknown send() command { 0:#x }", e.type.val);
                    break;
                }
            }

            uint8_t MidiDriver::calcVolume(const uint8_t channelVolume, uint8_t noteVolume) const noexcept
            {
                noteVolume = ((uint32_t)channelVolume * noteVolume) / (127);
                // TODO replace with Math.Min(noteVolume,127)
                if (noteVolume > 127)
                    return 127;
                else
                    return noteVolume;
            }

            void MidiDriver::releaseSustain(const uint8_t channel)
            {
                for (int i = 0; i < _oplNumChannels; i++)
                {
                    MidiVoice* voice = _voices[i].get();
                    if (voice->channel == channel && voice->sustain) {
                        releaseVoice(i, 0);
                    }
                }
            }

            uint8_t MidiDriver::releaseVoice(const uint8_t slot, const bool killed)
            {
                assert(slot >= 0 && slot < _oplNumChannels);

                MidiVoice* voice = _voices[slot].get();

                _playingVoices--;
                writeNote(voice, false);
                voice->free = true;
                if (killed)
                {
                    _oplWriter->writeChannel(0x80, slot, 0x0F, 0x0F);  // release rate - fastest
                    _oplWriter->writeChannel(0x40, slot, 0x3F, 0x3F);  // no volume
                }
                return slot;
            }

            int MidiDriver::allocateVoice(const uint8_t slot, const uint8_t channel, uint8_t note, uint8_t volume, const audio::opl::banks::Op2BankInstrument_t* instrument, const bool secondary, const uint32_t abs_time)
            {
                const OPL2instrument_t* instr;
                MidiChannel* data = _channels[channel].get();
                MidiVoice* voice = _voices[slot].get();

                _playingVoices++;

                voice->channel = channel;
                voice->note = note;
                voice->free = false;
                voice->secondary = secondary;
                if (data->_modulation >= VIBRATO_THRESHOLD)
                    voice->vibrato = true;
                voice->time = abs_time;
                voice->realvolume = calcVolume(data->_volume, voice->volume = volume);
                if (instrument->flags & OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH)
                    note = instrument->noteNum;
                else if (channel == MIDI_PERCUSSION_CHANNEL)
                    note = 60;			// C-5
                if (secondary && (instrument->flags & OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE))
                    voice->finetune = instrument->fineTune - 0x80;
                else
                    voice->finetune = 0;
                voice->pitch = voice->finetune + data->_pitch;
                if (secondary)
                    instr = &instrument->voices[1];
                else
                    instr = &instrument->voices[0];
                voice->instr = instr;
                if ((note += instr->basenote) < 0)
                    while ((note += 12) < 0);
                else if (note > HIGHEST_NOTE)
                    while ((note -= 12) > HIGHEST_NOTE);
                voice->realnote = note;

                // TODO: these can be voice->Allocate method
                _oplWriter->writeInstrument(slot, instr);
                if (voice->vibrato)
                    _oplWriter->writeModulation(slot, instr, 1);
                _oplWriter->writePan(slot, instr, data->_pan);
                _oplWriter->writeVolume(slot, instr, voice->realvolume);
                writeNote(voice, true);
                
                return slot;
            }

            int8_t MidiDriver::findFreeOplChannel(const uint8_t flag, const uint32_t abs_time)
            {
                uint8_t i;
                uint8_t oldest = 255;
                uint32_t oldesttime = abs_time;

                // find free channel
                for (i = 0; i < _oplNumChannels; i++)
                {
                    MidiVoice* voice = _voices[i].get();
                    if (voice->free)
                        return i;
                }

                if (flag & 1)
                    return -1;  // stop searching if bit 0 is set 

                // find some 2nd-voice channel and determine the oldest
                for (i = 0; i < _oplNumChannels; i++)
                {
                    MidiVoice* voice = _voices[i].get();
                    if (voice->secondary) {
                        return releaseVoice(i, true);
                    }
                    else if (voice->time < oldesttime) {
                        oldesttime = voice->time;
                        oldest = i;
                    }
                }

                // if possible, kill the oldest channel
                if (!(flag & 2) && oldest != 255)
                {
                    return releaseVoice(oldest, true);
                }

                return -1;
            }

            uint8_t MidiDriver::panVolume(const uint8_t volume, int8_t pan) const noexcept
            {
                return (pan >= 0) ?
                    volume :
                    (volume * (pan + 64)) >> 6; // / 64;
            }

            void MidiDriver::writeNote(const MidiVoice* voice, const bool keyOn) const noexcept
            {
                // TODO: keyOn put in MidiVoice?
                _oplWriter->writeNote(voice->_slot, voice->realnote, voice->pitch, keyOn);
            }
        }
    }
}
