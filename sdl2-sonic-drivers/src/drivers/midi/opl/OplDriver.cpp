#include <drivers/midi/opl/OplDriver.hpp>
#include <spdlog/spdlog.h>
#include <cassert>
#include <algorithm>

namespace drivers
{
    namespace midi
    {
        namespace opl
        {
            using audio::midi::MIDI_PERCUSSION_CHANNEL;
            using audio::midi::MIDI_EVENT_TYPES_HIGH;
            using hardware::opl::OPL2instrument_t;

            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_FIXED_PITCH;
            using audio::opl::banks::OP2BANK_INSTRUMENT_FLAG_DOUBLE_VOICE;


            // TODO: allocateVoice and getFreeSlot should be merged into 1 function

            OplDriver::OplDriver(const std::shared_ptr<hardware::opl::OPL>& opl, const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) :
                _opl(opl)
            {
                // TODO: force to be adlib now
                _oplWriter = std::make_unique<drivers::opl::OplWriter>(_opl, false);

                if (!_oplWriter->init())
                    spdlog::error("[MidiDriver] Can't initialize AdLib Emulator OPL chip.'");

                for (int i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i) {
                    _channels[i] = std::make_unique<OplChannel>(i, op2Bank);
                }

                for (int i = 0; i < _oplNumChannels; ++i) {
                    _voices[i] = std::make_unique<OplVoice>(i, _oplWriter);
                    _voiceIndexesFree.push_back(i);
                }

                hardware::opl::TimerCallBack cb = std::bind(&OplDriver::onTimer, this);
                auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
                _opl->start(p);
            }

            OplDriver::~OplDriver()
            {
                // deinit
                _oplWriter.reset();
                _opl->stop();
            }

            void OplDriver::onTimer()
            {
            }

            void OplDriver::send(const audio::midi::MIDIEvent& e) noexcept
            {
                // TODO: this one if it was the abs_time computed from delta_time
                //       was faster and better.
                // It looks like is not needed anymore.
                //uint32_t abs_time = getMillis<uint32_t>();

                switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
                {
                case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
                    noteOff(e.type.low, e.data[0]);
                    break;
                case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
                    noteOn(e.type.low, e.data[0], e.data[1]);
                    break;
                case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
                    spdlog::warn("AFTERTOUCH not supported");
                    break;
                case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
                    controller(e.type.low, e.data[0], e.data[1]);
                    break;
                case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
                    programChange(e.type.low, e.data[0]);
                    break;
                case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                    spdlog::warn("CHANNEL_AFTERTOUCH not supported");
                    break;
                case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                    pitchBend(e.type.low, (e.data[0] | (e.data[1] << 7) - 0x2000) >> 6);
                    break;
                case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
                    spdlog::warn("META_SYSEX not supported");
                    break;
                default:
                    spdlog::warn("MidiDriver: Unknown send() command { 0:#x }", e.type.val);
                    break;
                }
            }

            void OplDriver::noteOff(const uint8_t chan, const uint8_t note) noexcept
            {
                uint8_t sustain = _channels[chan]->sustain;

                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end();) {
                    // TODO: this noteOff is masking the voice Release, not nice.
                    if (_voices[*it]->noteOff(chan, note, sustain)) {
                        _voiceIndexesFree.push_back(*it);
                        it = _voiceIndexesInUse.erase(it);
                    }
                    else
                        ++it;
                }

                //spdlog::debug("noteOff {} {} ({})", chan, note, _voiceIndexesInUse.size());
            }

            void OplDriver::noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept
            {
                int8_t freeSlot = getFreeOplVoiceIndex(chan != MIDI_PERCUSSION_CHANNEL);

                if (freeSlot != -1)
                {
                    allocateVoice(freeSlot, chan, note, vol,
                        _channels[chan]->setInstrument(note), false);

                    // TODO: OPL3
                    //if (!OPLsinglevoice && instr->flags == FL_DOUBLE_VOICE)
                    //{
                    //    if ((i = findFreeChannel(mus, (channel == PERCUSSION) ? 3 : 1)) != -1)
                    //        occupyChannel(mus, i, channel, note, volume, instr, 1);
                    //}

                    //MidiVoice* voice = _voices[chi].get();
                    //spdlog::debug("noteOn note={:d} ({:d}) - vol={:d} ({:d}) - pitch={:d} - ch={:d}", voice->_note, voice->_realnote, /*voice->volume*/ -1, /*voice->realvolume*/ -1, voice->pitch, voice->_channel);
                }
                else {
                    spdlog::critical("NO FREE CHANNEL? midi-ch={} - playingVoices={}", chan, _voiceIndexesInUse.size());
                }
            }

            void OplDriver::controller(const uint8_t chan, const uint8_t control, uint8_t value) noexcept
            {
                // MIDI_EVENT_CONTROLLER_TYPES
                switch (control)
                {
                case 0:
                case 32:
                    // Bank select. Not supported
                    spdlog::warn("bank select value {}", value);
                    break;
                case 1:
                    ctrl_modulationWheel(chan, value);
                    //spdlog::debug("modwheel value {}", value);
                    break;
                case 7:
                    ctrl_volume(chan, value);
                    break;
                case 10:
                    // Not Available on OPL2/AdLib.
                    //ctrl_panPosition(chan, value, abs_time);
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
                    ctrl_sustain(chan, value);
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
                    //spdlog::debug("all notes off");
                    _oplWriter->stopAll();
                    break;
                default:
                    spdlog::warn("AdLib: Unknown control change message {:d} {:d}", control, value);
                }
            }

            void OplDriver::programChange(const uint8_t chan, const uint8_t program) const noexcept
            {
                _channels[chan]->programChange(program);
            }

            void OplDriver::pitchBend(const uint8_t chan, const uint16_t bend) const noexcept
            {
                //spdlog::debug("PITCH_BEND {}", bend);
                // OPLPitchWheel
                _channels[chan]->pitch = static_cast<int8_t>(bend);

                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end();++it)
                    _voices[*it]->pitchBend(chan, bend);
            }


            void OplDriver::ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept
            {
                _channels[chan]->modulation = value;

                for(auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end(); ++it)
                    _voices[*it]->ctrl_modulationWheel(chan, value);
            }

            void OplDriver::ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept
            {
                //spdlog::debug("volume value {} -ch={}", value, chan);

                _channels[chan]->volume = value;
                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end(); ++it)
                    _voices[*it]->ctrl_volume(chan, value/*, abs_time*/);
            }

            void OplDriver::ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept
            {
                //spdlog::debug("panPosition value {}", value);

                _channels[chan]->pan = value -= 64;
                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end(); ++it)
                    _voices[*it]->ctrl_panPosition(chan, value);
            }

            void OplDriver::ctrl_sustain(const uint8_t chan, uint8_t value) noexcept
            {
                //spdlog::debug("sustain value {}", value);
                _channels[chan]->sustain = value;
                if (value < SUSTAIN_THRESHOLD)
                    releaseSustain(chan);
            }

            void OplDriver::releaseSustain(const uint8_t channel)
            {
                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end(); ++it)
                    _voices[*it]->releaseSustain(*it);
            }

            uint8_t OplDriver::releaseVoice(const uint8_t slot, const bool killed)
            {
                assert(slot >= 0 && slot < _oplNumChannels);

                return _voices[slot]->release(killed);
            }

            int OplDriver::allocateVoice(const uint8_t slot, const uint8_t channel,
                const uint8_t note_, const uint8_t volume,
                const audio::opl::banks::Op2BankInstrument_t* instrument,
                const bool secondary/*, const uint32_t abs_time*/)
            {
                OplChannel* ch = _channels[channel].get();

                return _voices[slot]->allocate(
                    channel, note_, volume, instrument, secondary,
                    ch->modulation, ch->volume, ch->pitch, ch->pan
                );
            }

            int8_t OplDriver::getFreeOplVoiceIndex(const bool force)
            {
                assert(_voiceIndexesFree.size() + _voiceIndexesInUse.size() == _oplNumChannels);

                if (!_voiceIndexesFree.empty()) {
                    const uint8_t i = _voiceIndexesFree.front();
                    _voiceIndexesFree.pop_front();
                    _voiceIndexesInUse.push_back(i);
                    return i;
                }

                for (auto it = _voiceIndexesInUse.begin(); it != _voiceIndexesInUse.end(); ++it)
                {
                    if (_voices[*it]->isSecondary()) {
                        uint8_t i = releaseVoice(*it, true);
                        _voiceIndexesInUse.erase(it);
                        _voiceIndexesInUse.push_back(i);
                        return i;
                    }
                }

                if(force)
                {
                    uint8_t i = releaseVoice(_voiceIndexesInUse.front(), true);
                    _voiceIndexesInUse.pop_front();
                    _voiceIndexesInUse.push_back(i);
                    return i;
                }

                return -1;
            }
        }
    }
}
