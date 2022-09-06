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

            // TODO: allocateVoice and getFreeSlot should be merged into 1 function

            OplDriver::OplDriver(const std::shared_ptr<hardware::opl::OPL>& opl,
                const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank, const bool opl3_mode) :
                _opl(opl), _op2Bank(op2Bank), _opl3_mode(opl3_mode)
            {
                _oplWriter = std::make_unique<drivers::opl::OplWriter>(_opl, opl3_mode);
                _oplNumChannels = opl3_mode ? drivers::opl::OPL3_NUM_CHANNELS : drivers::opl::OPL2_NUM_CHANNELS;

                if (!_oplWriter->init())
                    spdlog::error("[MidiDriver] Can't initialize AdLib Emulator OPL chip.'");

                for (uint8_t i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i) {
                    _channels[i] = std::make_unique<OplChannel>(i);
                }

                for (uint8_t i = 0; i < _oplNumChannels; ++i) {
                    _voices[i] = std::make_unique<OplVoice>(i, _oplWriter.get());
                    _voicesFreeIndex.push_back(i);
                }

                hardware::opl::TimerCallBack cb = std::bind(&OplDriver::onTimer, this);
                auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
                _opl->start(p);
            }

            OplDriver::~OplDriver()
            {
                _opl->stop();
            }

            void OplDriver::onTimer()
            {
            }

            void OplDriver::send(const audio::midi::MIDIEvent& e) noexcept
            {
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
                    pitchBend(e.type.low, static_cast<uint16_t>((e.data[0] | (e.data[1] << 7) - 0x2000) >> 6));
                    break;
                case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
                    spdlog::warn("META_SYSEX not supported");
                    break;
                default:
                    spdlog::warn("MidiDriver: Unknown send() command { 0:#x }", e.type.val);
                    break;
                }
            }

            void OplDriver::pause() const noexcept
            {
                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it) {
                    const uint8_t i = *it;
                    if (_opl3_mode)
                        _oplWriter->writeValue(0xC0, i, _voices[i]->getInstrument()->feedback);
                    _voices[i]->pause();
                }
            }

            void OplDriver::resume() const noexcept
            {
                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it) {
                    const uint8_t i = *it;
                    _voices[i]->resume();
                    // TODO: the voice could be stereo
                    //       so it should have the pan i guess
                    if (_opl3_mode)
                    {
                        _oplWriter->writePan(i,
                            _voices[i]->getInstrument(),
                            _channels[_voices[i]->getChannel()]->pan
                        );
                    }
                }
            }

            void OplDriver::noteOff(const uint8_t chan, const uint8_t note) noexcept
            {
                const uint8_t sustain = _channels[chan]->sustain;

                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end();) {
                    // TODO: this noteOff is masking the voice Release, not nice.
                    if (_voices[*it]->noteOff(chan, note, sustain)) {
                        _voicesFreeIndex.push_back(*it);
                        it = _voicesInUseIndex.erase(it);
                    }
                    else
                        ++it;
                }

                //spdlog::debug("noteOff {} {} ({})", chan, note, _voiceIndexesInUse.size());
            }

            void OplDriver::noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept
            {
                using audio::opl::banks::OP2Bank;

                const bool isPercussion = chan == MIDI_PERCUSSION_CHANNEL;
                int8_t freeSlot = getFreeOplVoiceIndex(isPercussion);
                
                if (freeSlot != -1)
                {
                    const uint8_t instr_index = isPercussion ?
                        OP2Bank::getPercussionIndex(note) :
                        _channels[chan]->getProgram();

                    const auto instr = _op2Bank->getInstrumentPtr(instr_index);

                    allocateVoice(freeSlot, chan, note, vol, instr, false);

                    if (_opl3_mode && OP2Bank::supportOpl3(instr))
                    {
                        freeSlot = getFreeOplVoiceIndex(true);
                        if (freeSlot != -1)
                            allocateVoice(freeSlot, chan, note, vol, instr, true);
                    }

                    //spdlog::debug("noteOn note={:d} ({:d}) - vol={:d} ({:d}) - pitch={:d} - ch={:d}", voice->_note, voice->_realnote, /*voice->volume*/ -1, /*voice->realvolume*/ -1, voice->pitch, voice->_channel);
                }
                else {
                    spdlog::critical("NO FREE CHANNEL? midi-ch={} - playingVoices={} -- free={}", chan, _voicesInUseIndex.size(), _voicesFreeIndex.size());
                    for (const auto& i : _voicesFreeIndex) 
                        spdlog::debug("_voicesFreeIndex ={} - voice.free={}", i, _voices[i]->isFree());
                    for (const auto& i : _voicesInUseIndex)
                        spdlog::debug("_voicesInUseIndex={} - voice.free={}", i, _voices[i]->isFree());
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
                    if(_opl3_mode)
                        ctrl_panPosition(chan, value);
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

                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end();++it)
                    _voices[*it]->pitchBend(chan, bend);
            }


            void OplDriver::ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept
            {
                _channels[chan]->modulation = value;

                for(auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it)
                    _voices[*it]->ctrl_modulationWheel(chan, value);
            }

            void OplDriver::ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept
            {
                //spdlog::debug("volume value {} -ch={}", value, chan);

                _channels[chan]->volume = value;
                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it)
                    _voices[*it]->ctrl_volume(chan, value/*, abs_time*/);
            }

            void OplDriver::ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept
            {
                //spdlog::debug("panPosition value {}", value);

                _channels[chan]->pan = value -= 64;
                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it)
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
                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it)
                    _voices[*it]->releaseSustain(channel);
            }

            uint8_t OplDriver::releaseVoice(const uint8_t slot, const bool forced)
            {
                assert(slot >= 0 && slot < _oplNumChannels);

                return _voices[slot]->release(forced);
            }

            int OplDriver::allocateVoice(const uint8_t slot, const uint8_t channel,
                const uint8_t note, const uint8_t volume,
                const audio::opl::banks::Op2BankInstrument_t* instrument,
                const bool secondary)
            {
                const OplChannel* ch = _channels[channel].get();

                return _voices[slot]->allocate(
                    channel, note, volume, instrument, secondary,
                    ch->modulation, ch->volume, ch->pitch, ch->pan
                );
            }

            int8_t OplDriver::getFreeOplVoiceIndex(const bool force)
            {
                // TODO flag = 0, 1, 2, 3 ???  (using 2 bits)
                // 0 :=> search a free channel, and if not find try to kill a 2nd voice or oldest
                // 1 :=> search a free channel only.
                // 2 :=> like 0 but won't kill the oldest channel
                // 3 :=> is like 1 as it return when bit 0 is 1
                // -----
                // used only 0 and 2, so bit 1 only:
                // it can be a bool !kill_oldest_channel

                assert(_voicesFreeIndex.size() + _voicesInUseIndex.size() == _oplNumChannels);

                if (!_voicesFreeIndex.empty()) {
                    const uint8_t i = _voicesFreeIndex.front();
                    _voicesFreeIndex.pop_front();
                    _voicesInUseIndex.push_back(i);
                    return i;
                }

                for (auto it = _voicesInUseIndex.begin(); it != _voicesInUseIndex.end(); ++it)
                {
                    if (_voices[*it]->isSecondary()) {
                        uint8_t i = releaseVoice(*it, true);
                        _voicesInUseIndex.erase(it);
                        _voicesInUseIndex.push_back(i);
                        return i;
                    }
                }

                if(force)
                {
                    uint8_t i = releaseVoice(_voicesInUseIndex.front(), true);
                    _voicesInUseIndex.pop_front();
                    _voicesInUseIndex.push_back(i);
                    return i;
                }

                return -1;
            }
        }
    }
}
