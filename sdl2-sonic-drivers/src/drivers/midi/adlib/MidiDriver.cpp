#include <drivers/midi/adlib/MidiDriver.hpp>
#include <spdlog/spdlog.h>
#include<utils/algorithms.hpp>
#include <algorithm>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            using audio::midi::MIDI_PERCUSSION_CHANNEL;
            using hardware::opl::OPL2instrument_t;

            static uint16_t freqtable[] = {                                 /* note # */
                345, 365, 387, 410, 435, 460, 488, 517, 547, 580, 615, 651, /*  0 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 12 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 24 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 36 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 48 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 60 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 72 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 84 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 96 */
                690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 108 */
                690, 731, 774, 820, 869, 921, 975, 517 };                   /* 120 */

            static uint8_t octavetable[] = {        /* note # */
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  0 */
                0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, /* 12 */
                1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, /* 24 */
                2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, /* 36 */
                3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, /* 48 */
                4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, /* 60 */
                5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, /* 72 */
                6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, /* 84 */
                7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, /* 96 */
                8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, /* 108 */
                9, 9, 9, 9, 9, 9, 9,10 };           /* 120 */

            static uint16_t pitchtable[] = {                               /* pitch wheel */
                 29193U,29219U,29246U,29272U,29299U,29325U,29351U,29378U,  /* -128 */
                 29405U,29431U,29458U,29484U,29511U,29538U,29564U,29591U,  /* -120 */
                 29618U,29644U,29671U,29698U,29725U,29752U,29778U,29805U,  /* -112 */
                 29832U,29859U,29886U,29913U,29940U,29967U,29994U,30021U,  /* -104 */
                 30048U,30076U,30103U,30130U,30157U,30184U,30212U,30239U,  /*  -96 */
                 30266U,30293U,30321U,30348U,30376U,30403U,30430U,30458U,  /*  -88 */
                 30485U,30513U,30541U,30568U,30596U,30623U,30651U,30679U,  /*  -80 */
                 30706U,30734U,30762U,30790U,30817U,30845U,30873U,30901U,  /*  -72 */
                 30929U,30957U,30985U,31013U,31041U,31069U,31097U,31125U,  /*  -64 */
                 31153U,31181U,31209U,31237U,31266U,31294U,31322U,31350U,  /*  -56 */
                 31379U,31407U,31435U,31464U,31492U,31521U,31549U,31578U,  /*  -48 */
                 31606U,31635U,31663U,31692U,31720U,31749U,31778U,31806U,  /*  -40 */
                 31835U,31864U,31893U,31921U,31950U,31979U,32008U,32037U,  /*  -32 */
                 32066U,32095U,32124U,32153U,32182U,32211U,32240U,32269U,  /*  -24 */
                 32298U,32327U,32357U,32386U,32415U,32444U,32474U,32503U,  /*  -16 */
                 32532U,32562U,32591U,32620U,32650U,32679U,32709U,32738U,  /*   -8 */
                 32768U,32798U,32827U,32857U,32887U,32916U,32946U,32976U,  /*    0 */
                 33005U,33035U,33065U,33095U,33125U,33155U,33185U,33215U,  /*    8 */
                 33245U,33275U,33305U,33335U,33365U,33395U,33425U,33455U,  /*   16 */
                 33486U,33516U,33546U,33576U,33607U,33637U,33667U,33698U,  /*   24 */
                 33728U,33759U,33789U,33820U,33850U,33881U,33911U,33942U,  /*   32 */
                 33973U,34003U,34034U,34065U,34095U,34126U,34157U,34188U,  /*   40 */
                 34219U,34250U,34281U,34312U,34343U,34374U,34405U,34436U,  /*   48 */
                 34467U,34498U,34529U,34560U,34591U,34623U,34654U,34685U,  /*   56 */
                 34716U,34748U,34779U,34811U,34842U,34874U,34905U,34937U,  /*   64 */
                 34968U,35000U,35031U,35063U,35095U,35126U,35158U,35190U,  /*   72 */
                 35221U,35253U,35285U,35317U,35349U,35381U,35413U,35445U,  /*   80 */
                 35477U,35509U,35541U,35573U,35605U,35637U,35669U,35702U,  /*   88 */
                 35734U,35766U,35798U,35831U,35863U,35895U,35928U,35960U,  /*   96 */
                 35993U,36025U,36058U,36090U,36123U,36155U,36188U,36221U,  /*  104 */
                 36254U,36286U,36319U,36352U,36385U,36417U,36450U,36483U,  /*  112 */
                 36516U,36549U,36582U,36615U,36648U,36681U,36715U,36748U }; /*  120 */


            // TODO: there is a bug that cannot find a channel when max channel shoudl be just 5
            //       infact there is no gtr2 playing ... looks like
            //       probably is due during the initialization


            // TODO: clean up and refactor the class
            // TODO: split up in MidiChannel and MidiVoice etc.. (AdlibChannel, AdlibVoice ??)

            // TODO: when no channel is allocated having a for loop to search for nothing is silly.
            // TODO: would make sense to use an hashmap instead no for loop if it is present in the hashmap
            //       ok, if not present is not in use


            // TODO: review the old C logic of the #define and FLAGS using bool instead for e.g.



            MidiDriver::MidiDriver(std::shared_ptr<hardware::opl::OPL> opl, std::shared_ptr<files::dmx::OP2File> op2file) :
                _opl(opl), _op2file(op2file)
            {
                // TODO: need to initialize the channels with the instruments
                // TODO: need to pass the GENMIDI.OP2 read file to init the instruments
                // TODO: otherwise looks there is no sound.
                init();

                for (int i = 0; i < _oplNumChannels; ++i) {
                    memset(&_oplChannels[i], 0, sizeof(channelEntry));
                    _oplChannels[i].flags = CH_FREE;
                    _oplChannels[i].instr = &_op2file->getInstrument(0).voices[0];
                }

                for (int i = 0; i < audio::midi::MIDI_MAX_CHANNELS; ++i) {
                    _oplData.channelInstr[i] = 0;
                    _oplData.channelLastVolume[i] = 0;
                    _oplData.channelModulation[i] = 0;
                    _oplData.channelPan[i] = 0;
                    _oplData.channelPitch[i] = 0;
                    _oplData.channelSustain[i] = 0;
                    _oplData.channelVolume[i] = 0;
                }

                // TODO: not sure the callback is required yet...
                hardware::opl::TimerCallBack cb = std::bind(&MidiDriver::onTimer, this);
                auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
                _opl->start(p);
            }

            MidiDriver::~MidiDriver()
            {
                // deinit
                stopAll();
                _opl->writeReg(0x01, 0x20); // enable Waveform Select
                _opl->writeReg(0x08, 0x00); // turn off CSW mode
                _opl->writeReg(0xBD, 0x00); // set vibrato/tremolo depth to low, set melodic mode
            }

            void MidiDriver::onTimer()
            {
                int i = 0;
            }

            void MidiDriver::send(const audio::midi::MIDIEvent& e) noexcept
            {
                using audio::midi::MIDI_EVENT_TYPES_HIGH;
                
                switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
                {
                case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
                {
                    uint8_t chan = e.type.low;
                    uint8_t note = e.data[0];
                    //writeValue(0xB0, chan, 0);  // KEY-OFF
                    //_channels[chan].noteOff(note);


                    //uint i;
                    //uint id = MAKE_ID(channel, mus->number);
                    OPLdata* data = &_oplData;
                    uint8_t sustain = data->channelSustain[chan];

                    for (int i = 0; i < _oplNumChannels; i++) {
                        //if (CHANNEL_ID(channels[i]) == id && channels[i].note == note)
                        if (_oplChannels[i].note == note && _oplChannels[i].channel == chan)
                        {
                            if (sustain < 0x40)
                                releaseChannel(i, 0);
                            else
                                _oplChannels[i].flags |= CH_SUSTAIN;
                        }
                    }

                    spdlog::debug("noteOff {} {}", chan, note);
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
                {
                    uint8_t chan = e.type.low;
                    uint8_t note = e.data[0];
                    uint8_t volume = e.data[1];
                    int8_t freeSlot = 0;
                    auto instr = getInstrument(chan, note);
                    if ((freeSlot = findFreeOplChannel((chan == MIDI_PERCUSSION_CHANNEL) ? 2 : 0, e.abs_time)) != -1)
                    {
                        int chi = occupyChannel(freeSlot, chan, note, volume, instr, 0, e.abs_time);

                        // TODO: OPL3
                        //if (!OPLsinglevoice && instr->flags == FL_DOUBLE_VOICE)
                        //{
                        //    if ((i = findFreeChannel(mus, (channel == PERCUSSION) ? 3 : 1)) != -1)
                        //        occupyChannel(mus, i, channel, note, volume, instr, 1);
                        //}

                        spdlog::debug("noteOn note={:d} ({:d}) - vol={:d} ({:d}) - pitch={:d} - ch={:d}", _oplChannels[chi].note, _oplChannels[chi].realnote, _oplChannels[chi].volume, _oplChannels[chi].realvolume, _oplChannels[chi].pitch, _oplChannels[chi].channel);
                    }
                    else {
                        spdlog::critical("NO FREE CHANNEL?");
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
                        spdlog::warn("bank select value {}", value);
                        // Bank select. Not supported
                        break;
                    case 1:
                    {
                        _oplData.channelModulation[chan] = value;
                        for (int i = 0; i < _oplNumChannels; i++)
                        {
                            channelEntry* ch = &_oplChannels[i];
                            //if (CHANNEL_ID(*ch) == id)
                            if (ch->channel == chan && (ch->flags & CH_FREE) == 0)
                            {
                                uint8_t flags = ch->flags;
                                ch->time = e.abs_time;
                                if (value >= MOD_MIN)
                                {
                                    ch->flags |= CH_VIBRATO;
                                    if (ch->flags != flags)
                                        writeModulation(i, ch->instr, 1);
                                }
                                else {
                                    ch->flags &= ~CH_VIBRATO;
                                    if (ch->flags != flags)
                                        writeModulation(i, ch->instr, 0);
                                }
                            }
                        }
                    }
                        spdlog::debug("modwheel value {}", value);
                        //modulationWheel(value);
                        break;
                    case 7:
                        {
                            int i;
                            _oplData.channelVolume[chan] = value;
                            for (i = 0; i < _oplNumChannels; i++)
                            {
                                channelEntry* ch = &_oplChannels[i];
                                //if (CHANNEL_ID(*ch) == id)
                                if (ch->channel == chan && (ch->flags & CH_FREE) == 0)
                                {
                                    ch->time = e.abs_time;
                                    ch->realvolume = calcVolume(value, ch->volume);
                                    writeVolume(i, ch->instr, ch->realvolume);
                                }
                            }
                            spdlog::debug("volume value {} -ch={}", value, i);

                        }
                        break;
                    case 10:
                        spdlog::warn("panPosition value {}", value);
                        //panPosition(value);
                        {
                            _oplData.channelPan[chan] = value -= 64;
                            for (int i = 0; i < _oplNumChannels; i++)
                            {
                                channelEntry* ch = &_oplChannels[i];
                                //if (CHANNEL_ID(*ch) == id)
                                if (ch->channel == chan && (ch->flags & CH_FREE) == 0)
                                {
                                    ch->time = e.abs_time;
                                    writePan(i, ch->instr, value);
                                }
                            }
                        }
                        break;
                    case 16:
                        spdlog::warn("pitchBendFactor value {}", value);
                        //pitchBendFactor(value);
                        break;
                    case 17:
                        spdlog::warn("detune value {}", value);
                        //detune(value);
                        break;
                    case 18:
                        spdlog::warn("priority value {}", value);
                        //priority(value);
                        break;
                    case 64:
                        spdlog::warn("sustain value {}", value);
                        //sustain(value > 0);
                        {
                            _oplData.channelSustain[chan] = value;
                            if (value < 0x40) {
                                releaseSustain(chan);
                            }
                        }
                        break;
                    case 91:
                        // Effects level. Not supported.
                        spdlog::warn("effect level value {}", value);
                        //effectLevel(value);
                        break;
                    case 93:
                        // Chorus level. Not supported.
                        spdlog::warn("chorus level value {}", value);
                        //chorusLevel(value);
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
                        //allNotesOff();
                        stopAll();
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

                    //if (program > 127)
                    //    return;
                    _instruments[chan] = _op2file->getInstrument(program);
                    //writeInstrument(chan, &_instruments[chan].voices[0]);
                    _oplData.channelInstr[chan] = program;
                    // TODO with channels, later
                    //_channels[chan].programChange(program, _op2file->getInstrument(program));
                    //writeInstrument(chan, &_channels[chan].getInstrument()->voices[0]);

                    spdlog::debug("program change {} {} ({})", chan, program, _op2file->getInstrumentName(program));
                }
                    break;
                case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                    spdlog::warn("CHANNEL_AFTERTOUCH not supported");
                    break;
                case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                {
                    uint8_t chan = e.type.low;
                    uint16_t bend = (e.data[0] | (e.data[1] << 7));
                    spdlog::debug("PITCH_BEND {}", bend);

                    // OPLPitchWheel
                    //uint i;
                    // uint id = MAKE_ID(channel, mus->number);
                    //struct OPLdata* data = (struct OPLdata*)mus->driverdata;

                    _oplData.channelPitch[chan] = static_cast<int8_t>(bend/64);
                    for (int i = 0; i < _oplNumChannels; i++)
                    {
                        channelEntry* ch = &_oplChannels[i];
                        //if (CHANNEL_ID(*ch) == id)
                        if (ch->channel == chan)
                        {
                            ch->time = e.abs_time;
                            ch->pitch = ch->finetune + bend;
                            writeNote(i, ch->realnote, ch->pitch, 1);
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

            void MidiDriver::init() const noexcept
            {
                // TODO detect if OPL is a OPL2 ? 
                // TODO this need to refactor the opl namespaces etc..
                if (!_opl->init()) {
                    spdlog::error("[MidiDriver] unable to initialize OPL");
                }

                // Init Adlib
                _opl->writeReg(0x01, 0x20); // enable Waveform Select
                _opl->writeReg(0x08, 0x40); // turn off CSW mode
                _opl->writeReg(0xBD, 0x00); // set vibrato/tremolo depth to low, set melodic mode

                stopAll();
            }

            void MidiDriver::stopAll() const noexcept
            {
                for (int i = 0; i < _oplNumChannels; i++)
                {
                    writeChannel(0x40, i, 0x3F, 0x3F);  // turn off volume
                    writeChannel(0x60, i, 0xFF, 0xFF);  // the fastest attack, decay
                    writeChannel(0x80, i, 0x0F, 0x0F);  // ... and release
                    writeValue(0xB0, i, 0);             // KEY-OFF
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
                //uint i;
                //uint id = MAKE_ID(channel, mus->number);

                for (int i = 0; i < _oplNumChannels; i++)
                {
                    //if (CHANNEL_ID(channels[i]) == id && channels[i].flags & CH_SUSTAIN)
                    if (_oplChannels[i].channel == i && _oplChannels[i].flags & CH_SUSTAIN) {
                        releaseChannel(i, 0);
                    }
                }
            }

            uint8_t MidiDriver::releaseChannel(const uint8_t slot, const bool killed)
            {
                struct channelEntry* ch = &_oplChannels[slot];

                _playingChannels--;
                writeNote(slot, ch->realnote, ch->pitch, 0);
                //ch->channel |= CH_FREE;
                ch->flags = CH_FREE;
                if (killed)
                {
                    writeChannel(0x80, slot, 0x0F, 0x0F);  // release rate - fastest
                    writeChannel(0x40, slot, 0x3F, 0x3F);  // no volume
                }
                return slot;
            }

            int MidiDriver::occupyChannel(const uint8_t slot, const uint8_t channel, uint8_t note, uint8_t volume, files::dmx::OP2File::instrument_t* instrument, const uint8_t secondary, const uint32_t abs_time)
            {
                OPL2instrument_t* instr;
                OPLdata* data = &_oplData;
                channelEntry* ch = &_oplChannels[slot];

                _playingChannels++;

                ch->channel = channel;
                //ch->musnumber = mus->number;
                ch->note = note;
                ch->flags = secondary ? CH_SECONDARY : 0;
                if (data->channelModulation[channel] >= MOD_MIN)
                    ch->flags |= CH_VIBRATO;
                ch->time = abs_time;
                if (volume == -1)
                    volume = data->channelLastVolume[channel]; // TODO: unreachabel as midi always has volume
                else
                    data->channelLastVolume[channel] = volume;
                ch->realvolume = calcVolume(data->channelVolume[channel], ch->volume = volume);
                if (instrument->flags & FL_FIXED_PITCH)
                    note = instrument->noteNum;
                else if (channel == MIDI_PERCUSSION_CHANNEL)
                    note = 60;			// C-5
                if (secondary && (instrument->flags & FL_DOUBLE_VOICE))
                    ch->finetune = instrument->fineTune - 0x80;
                else
                    ch->finetune = 0;
                ch->pitch = ch->finetune + data->channelPitch[channel];
                if (secondary)
                    instr = &instrument->voices[1];
                else
                    instr = &instrument->voices[0];
                ch->instr = instr;
                if ((note += instr->basenote) < 0)
                    while ((note += 12) < 0);
                else if (note > HIGHEST_NOTE)
                    while ((note -= 12) > HIGHEST_NOTE);
                ch->realnote = note;

                writeInstrument(slot, instr);
                if (ch->flags & CH_VIBRATO)
                    writeModulation(slot, instr, 1);
                writePan(slot, instr, data->channelPan[channel]);
                writeVolume(slot, instr, ch->realvolume);
                writeNote(slot, note, ch->pitch, true);
                
                return slot;
            }

            int8_t MidiDriver::findFreeOplChannel(const uint8_t flag, const uint32_t abs_time)
            {
                //uint8_t last = -1U;
                uint8_t i;
                uint8_t oldest = -1U;
                uint32_t oldesttime = abs_time;

                /* find free channel */
                for (i = 0; i < _oplNumChannels; i++)
                {
                    //(++last) %= _oplNumChannels;
                    //if (++last == _oplNumChannels) {
                    //    /* use cyclic `Next Fit' algorithm */
                    //    last = 0;
                    //}
                    if (_oplChannels[i].flags & CH_FREE)
                        return i;
                }

                if (flag & 1)
                    return -1;			/* stop searching if bit 0 is set */

                /* find some 2nd-voice channel and determine the oldest */
                for (i = 0; i < _oplNumChannels; i++)
                {
                    if (_oplChannels[i].flags & CH_SECONDARY)
                    {
                        releaseChannel(i, true);
                        return i;
                    }
                    else
                        if (_oplChannels[i].time < oldesttime)
                        {
                            oldesttime = _oplChannels[i].time;
                            oldest = i;
                        }
                }

                /* if possible, kill the oldest channel */
                if (!(flag & 2) && oldest != -1U)
                {
                    releaseChannel(oldest, true);
                    return oldest;
                }

                return -1;
            }

            files::dmx::OP2File::instrument_t* MidiDriver::getInstrument(const uint8_t chan, const uint8_t note)
            {
                if (chan == MIDI_PERCUSSION_CHANNEL)
                {
                    if (note < 35 || note > 81) {
                        spdlog::error("wrong percussion number {}", note);
                    }
                    //i = note + (128 - 35);
                    auto instr = _op2file->getInstrument(note + (128 - 35));
                    memcpy(&_instruments[chan], &instr, sizeof(OPL2instrument_t));
                    //return &_instruments[chan];
                }
                else {
                    //return &_instruments[chan];
                }

                return &_instruments[chan];
            }

            void MidiDriver::writeChannel(const uint16_t regbase, const uint8_t channel, const uint8_t data1, const uint8_t data2) const noexcept
            {
                // OPL3 compatible channels
                static uint16_t op_num[] = {
                0x000, 0x001, 0x002, 0x008, 0x009, 0x00A, 0x010, 0x011, 0x012,
                0x100, 0x101, 0x102, 0x108, 0x109, 0x10A, 0x110, 0x111, 0x112 };

                uint16_t reg = regbase + op_num[channel];
                _opl->writeReg(reg, data1);
                _opl->writeReg(reg + 3, data2);
            }

            void MidiDriver::writeValue(const uint16_t regbase, const uint8_t channel, const uint8_t value) const noexcept
            {
                // OPL3 compatible channels
                static uint16_t reg_num[] = {
                0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008,
                0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108 };

                _opl->writeReg(regbase + reg_num[channel], value);
            }

            void MidiDriver::writeInstrument(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr) const noexcept
            {
                writeChannel(0x40, channel, 0x3F, 0x3F);    // no volume
                writeChannel(0x20, channel, instr->trem_vibr_1, instr->trem_vibr_2);
                writeChannel(0x60, channel, instr->att_dec_1, instr->att_dec_2);
                writeChannel(0x80, channel, instr->sust_rel_1, instr->sust_rel_2);
                writeChannel(0xE0, channel, instr->wave_1, instr->wave_2);
                writeValue(0xC0, channel, instr->feedback | 0x30);
            }

            void MidiDriver::writeModulation(const uint8_t slot, const hardware::opl::OPL2instrument_t* instr, uint8_t state)
            {
                if (state)
                    state = 0x40;	/* enable Frequency Vibrato */
                writeChannel(0x20, slot,
                    (instr->feedback & 1) ? (instr->trem_vibr_1 | state) : instr->trem_vibr_1,
                    instr->trem_vibr_2 | state);
            }

            void MidiDriver::writePan(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const int8_t pan) const noexcept
            {
                uint8_t bits;
                if (pan < -36) bits = 0x10;     // left
                else if (pan > 36) bits = 0x20; // right
                else bits = 0x30;               // both

                writeValue(0xC0, channel, instr->feedback | bits);
            }

            void MidiDriver::writeVolume(const uint8_t channel, const hardware::opl::OPL2instrument_t* instr, const uint8_t volume) const noexcept
            {
                writeChannel(0x40, channel, ((instr->feedback & 1) ?
                    convertVolume(instr->level_1, volume) : instr->level_1) | instr->scale_1,
                    convertVolume(instr->level_2, volume) | instr->scale_2);
            }

            uint8_t MidiDriver::convertVolume(const uint8_t data, const uint8_t volume) const noexcept
            {
                static uint8_t volumetable[128] = {
                  0,   1,   3,   5,   6,   8,  10,  11,
                 13,  14,  16,  17,  19,  20,  22,  23,
                 25,  26,  27,  29,  30,  32,  33,  34,
                 36,  37,  39,  41,  43,  45,  47,  49,
                 50,  52,  54,  55,  57,  59,  60,  61,
                 63,  64,  66,  67,  68,  69,  71,  72,
                 73,  74,  75,  76,  77,  79,  80,  81,
                 82,  83,  84,  84,  85,  86,  87,  88,
                 89,  90,  91,  92,  92,  93,  94,  95,
                 96,  96,  97,  98,  99,  99, 100, 101,
                101, 102, 103, 103, 104, 105, 105, 106,
                107, 107, 108, 109, 109, 110, 110, 111,
                112, 112, 113, 113, 114, 114, 115, 115,
                116, 117, 117, 118, 118, 119, 119, 120,
                120, 121, 121, 122, 122, 123, 123, 123,
                124, 124, 125, 125, 126, 126, 127, 127 };

                return 0x3F - (((0x3F - data) *
                    static_cast<uint8_t>(volumetable[volume <= 127 ? volume : 127]) >> 7));
            }

            uint8_t MidiDriver::panVolume(const uint8_t volume, int8_t pan) const noexcept
            {
                return (pan >= 0) ?
                    volume :
                    (volume * (pan + 64)) >> 6; // / 64;
            }

            void MidiDriver::writeFreq(const uint8_t channel, const uint16_t freq, const uint8_t octave, const bool keyon) const noexcept
            {
                writeValue(0xA0, channel, freq);
                writeValue(0xB0, channel, (freq >> 8) | (octave << 2) | (static_cast<uint8_t>(keyon) << 5));
            }

            void MidiDriver::writeNote(const uint8_t channel, const uint8_t note, int pitch, const bool keyOn) const noexcept
            {
                uint16_t freq = freqtable[note];
                uint8_t octave = octavetable[note];

                if (pitch != 0)
                {
                    pitch = std::clamp(pitch, -128, 127);
                    freq = static_cast<uint16_t>((static_cast<uint32_t>(freq) * pitchtable[pitch + 128]) >> 15);
                    if (freq >= 1024)
                    {
                        freq >>= 1;
                        octave++;
                    }
                }
                if (octave > 7) {
                    octave = 7;
                }
                
                writeFreq(channel, freq, octave, keyOn);
            }
        }
    }
}
