#include <drivers/westwood/ADLDriver.hpp>
#include <cstring>
#include <spdlog/spdlog.h>
#include <cassert>

namespace drivers
{
    namespace westwood
    {
        // TODO: remove
        static inline uint16_t READ_LE_UINT16(const void* ptr) {
            const uint8_t* b = (const uint8_t*)ptr;
            return (b[1] << 8) + b[0];
        }

        constexpr int NUM_CHANNELS = 9;
        constexpr int RANDOM_SEED = 0x1234;
        constexpr int RANDOM_INC = 0x9248;

        // TODO: move in utils
        template <class T>
        static inline T CLIP(const T& value, const T& min, const T& max) {
            return value < min ? min : value > max ? max : value;
        }

        ADLDriver::ADLDriver(hardware::opl::OPL* opl) :
            _opl(opl), _rnd(RANDOM_SEED)
        {
            memset(_channels, 0, sizeof(_channels));
        }

        ADLDriver::~ADLDriver()
        {
        }

        void ADLDriver::initDriver()
        {
            const std::lock_guard<std::mutex> lock(_mutex);
            resetAdLibState();
        }

        void ADLDriver::setSoundData(uint8_t* data, const uint32_t size)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            // Drop all tracks that are still queued.
            // These would point to the old sound data.
            _programQueueStart = _programQueueEnd = 0;
            _programQueue[0] = QueueEntry();

            _sfxPointer = nullptr;

            _soundData = data;
            _soundDataSize = size;
        }

        void ADLDriver::startSound(const int track, const int volume)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            uint8_t* trackData = getProgram(track);
            if (!trackData)
                return;

            if (_programQueueEnd == _programQueueStart && _programQueue[_programQueueEnd].data != 0)
            {
                // Don't warn when dropping tracks in EoB.
                // The queue is always full there if a couple of monsters are around.
                if (_version >= 3) {
                    spdlog::warn("ADLDriver: Program queue full, dropping track %d", track);
                }

                return;
            }

            _programQueue[_programQueueEnd] = QueueEntry(trackData, track, volume);
            ++_programQueueEnd &= 15;
        }

        bool ADLDriver::isChannelPlaying(const int channel)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            assert(channel >= 0 && channel <= NUM_CHANNELS);
            return (_channels[channel].dataptr != 0);
        }

        void ADLDriver::stopAllChannels()
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            for (int channel = 0; channel <= NUM_CHANNELS; ++channel)
            {
                _curChannel = channel;

                Channel& chan = _channels[_curChannel];
                chan.priority = 0;
                chan.dataptr = 0;

                if (channel != NUM_CHANNELS)
                    noteOff(chan);
            }
            _retrySounds = false;

            _programQueueStart = _programQueueEnd = 0;
            _programQueue[0] = QueueEntry();
            _programStartTimeout = 0;
        }

        int ADLDriver::getSoundTrigger() const
        {
            return _soundTrigger;
        }

        void ADLDriver::resetSoundTrigger()
        {
            _soundTrigger = 0;
        }

        void ADLDriver::callback()
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            if (_programStartTimeout) {
                --_programStartTimeout;
            }
            else {
                setupPrograms();
            }

            executePrograms();

            if (advance(_callbackTimer, _tempo))
            {
                if (!(--_beatDivCnt))
                {
                    _beatDivCnt = _beatDivider;
                    ++_beatCounter;
                }
            }
        }

        void ADLDriver::setVersion(const uint8_t v)
        {
            // TODO: get this from the ADL file instead.
            _version = v;
            _numPrograms = (_version == 1) ? 150 : ((_version == 4) ? 500 : 250);
        }

        void ADLDriver::setupPrograms()
        {
            QueueEntry& entry = _programQueue[_programQueueStart];
            uint8_t* ptr = entry.data;

            // If there is no program queued, we skip this.
            if (_programQueueStart == _programQueueEnd && !ptr) {
                return;
            }

            // The AdLib driver (in its old versions used for EOB) is not suitable for modern (fast) CPUs.
            // The stop sound track (track 0 which has a priority of 50) will often still be busy when the
            // next sound (with a lower priority) starts which will cause that sound to be skipped. We simply
            // restart incoming sounds during stop sound execution.
            // UPDATE: This still applies after introduction of the _programQueue.
            // UPDATE: This can also happen with the HOF main menu, so I commented out the version < 3 limitation.
            QueueEntry retrySound;
            if (/*_version < 3 &&*/ entry.id == 0)
                _retrySounds = true;
            else if (_retrySounds)
                retrySound = entry;

            // Clear the queue entry
            entry.data = nullptr;
            ++_programQueueStart &= 15;

            // Safety check: 2 bytes (channel, priority) are required for each
            // program, plus 2 more bytes (opcode, _sfxVelocity) for sound effects.
            // More data is needed, but executePrograms() checks for that.
            // Also ignore request for invalid channel number.
            if (!checkDataOffset(ptr, 2)) {
                return;
            }

            const int chan = *ptr;
            if (chan > NUM_CHANNELS || (chan < NUM_CHANNELS && !checkDataOffset(ptr, 4))) {
                return;
            }

            Channel& channel = _channels[chan];

            // Adjust data in case we hit a sound effect.
            adjustSfxData(ptr++, entry.volume);

            const int priority = *ptr++;

            // Only start this sound if its priority is higher than the one
            // already playing.
            if (priority >= channel.priority)
            {
                initChannel(channel);
                channel.priority = priority;
                channel.dataptr = ptr;
                channel.tempo = 0xFF;
                channel.timer = 0xFF;
                channel.duration = 1;

                if (chan <= 5) {
                    channel.volumeModifier = _musicVolume;
                }
                else {
                    channel.volumeModifier = _sfxVolume;
                }

                initAdlibChannel(chan);

                // We need to wait two callback calls till we can start another track.
                // This is (probably) required to assure that the sfx are started with
                // the correct priority and velocity.
                _programStartTimeout = 2;

                retrySound = QueueEntry();
            }

            if (retrySound.data)
            {
                spdlog::info("ADLDriver::setupPrograms(): WORKAROUND - Restarting skipped sound %d)", retrySound.id);
                startSound(retrySound.id, retrySound.volume);
            }
        }

        void ADLDriver::executePrograms()
        {
            // Each channel runs its own program. There are ten channels: One for
            // each AdLib channel (0-8), plus one "control channel" (9) which is
            // the one that tells the other channels what to do.
            if (_syncJumpMask)
            {
                // This is where we ensure that channels that are made to jump
                // "in sync" do so.

                for (_curChannel = NUM_CHANNELS; _curChannel >= 0; --_curChannel)
                {
                    if ((_syncJumpMask & (1 << _curChannel)) && _channels[_curChannel].dataptr && !_channels[_curChannel].lock) {
                        break; // don't unlock
                    }
                }

                if (_curChannel < 0)
                {
                    // force unlock
                    for (_curChannel = 9; _curChannel >= 0; --_curChannel) {
                        if (_syncJumpMask & (1 << _curChannel)) {
                            _channels[_curChannel].lock = false;
                        }
                    }
                }
            }

            for (_curChannel = NUM_CHANNELS; _curChannel >= 0; --_curChannel)
            {
                Channel& channel = _channels[_curChannel];
                const uint8_t*& dataptr = channel.dataptr;

                if (!dataptr) {
                    continue;
                }

                if (channel.lock && (_syncJumpMask & (1 << _curChannel))) {
                    continue;
                }

                if (_curChannel == NUM_CHANNELS) {
                    _curRegOffset = 0;
                }
                else {
                    _curRegOffset = _regOffset[_curChannel];
                }

                if (channel.tempoReset) {
                    channel.tempo = _tempo;
                }

                int result = 1;
                if (advance(channel.timer, channel.tempo))
                {
                    if (--channel.duration)
                    {
                        if (channel.duration == channel.spacing2) {
                            noteOff(channel);
                        }
                        if (channel.duration == channel.spacing1 && _curChannel != NUM_CHANNELS) {
                            noteOff(channel);
                        }
                    }
                    else {
                        // Process some opcodes.
                        result = 0;
                    }
                }

                while (result == 0 && dataptr)
                {
                    uint8_t opcode = 0xFF;
                    // Safety check to avoid illegal access.
                    // Stop channel if not enough data.
                    if (checkDataOffset(dataptr, 1)) {
                        opcode = *dataptr++;
                    }

                    if (opcode & 0x80)
                    {
                        opcode = CLIP(opcode & 0x7F, 0, _parserOpcodeTableSize - 1);
                        const ParserOpcode& op = _parserOpcodeTable[opcode];

                        // Safety check for end of data.
                        if (!checkDataOffset(dataptr, op.values)) {
                            result = update_stopChannel(channel, dataptr);
                            break;
                        }

                        spdlog::debug("Calling opcode '%s' (%d) (channel: %d)", op.name, opcode, _curChannel);
                        dataptr += op.values;
                        result = (this->*(op.function))(channel, dataptr - op.values);
                    }
                    else
                    {
                        // Safety check for end of data.
                        if (!checkDataOffset(dataptr, 1)) {
                            result = update_stopChannel(channel, dataptr);
                            break;
                        }

                        uint8_t duration = *dataptr++;
                        //debugC(9, kDebugLevelSound, "Note on opcode 0x%02X (duration: %d) (channel: %d)", opcode, duration, _curChannel);
                        spdlog::debug("Note on opcode 0x%02X (duration: %d) (channel: %d)", opcode, duration, _curChannel);
                        setupNote(opcode, channel);
                        noteOn(channel);
                        setupDuration(duration, channel);
                        // We need to make sure we are always running the
                        // effects after this. Otherwise some sounds are
                        // wrong. Like the sfx when bumping into a wall in
                        // LoL.
                        result = duration != 0;
                    }
                }

                if (result == 1)
                {
                    if (channel.primaryEffect) {
                        (this->*(channel.primaryEffect))(channel);
                    }
                    if (channel.secondaryEffect) {
                        (this->*(channel.secondaryEffect))(channel);
                    }
                }
            }
        }

        void ADLDriver::resetAdLibState()
        {
            spdlog::debug("resetAdLibState()");
            _rnd = RANDOM_SEED;
            // Authorize the control of the waveforms
            writeOPL(0x01, 0x20);
            // Select FM music mode
            writeOPL(0x08, 0x00);
            // I would guess the main purpose of this is to turn off the rhythm,
            // thus allowing us to use 9 melodic voices instead of 6.
            writeOPL(0xBD, 0x00);

            initChannel(_channels[NUM_CHANNELS]);
            for (int loop = 8; loop >= 0; loop--)
            {
                // Silence the channel
                writeOPL(0x40 + _regOffset[loop], 0x3F);
                writeOPL(0x43 + _regOffset[loop], 0x3F);
                initChannel(_channels[loop]);
            }
        }

        /// <summary>
        /// New calling style: writeOPL(0xAB, 0xCD)
        /// </summary>
        /// <param name="reg"></param>
        /// <param name="val"></param>
        void ADLDriver::writeOPL(const uint8_t reg, const uint8_t val)
        {
            _opl->write(reg, val);
        }

        void ADLDriver::initChannel(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "initChannel(%lu)", (long)(&channel - _channels));
            spdlog::debug("initChannel(%lu)", static_cast<long>(&channel - _channels));
            
            uint8_t backupEL2 = channel.opExtraLevel2;
            memset(&channel, 0, sizeof(Channel));

            channel.opExtraLevel2 = backupEL2;
            channel.tempo = 0xFF;
            channel.priority = 0;
            // normally here are nullfuncs but we set nullptr for now
            channel.primaryEffect = nullptr;
            channel.secondaryEffect = nullptr;
            channel.spacing1 = 1;
            channel.lock = false;
        }

        void ADLDriver::noteOff(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "noteOff(%lu)", (long)(&channel - _channels));
            spdlog::debug("noteOff(%lu)", static_cast<long>(&channel - _channels));

            // The control channel has no corresponding AdLib channel

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            // When the rhythm section is enabled, channels 6, 7 and 8 are special.
            if (_rhythmSectionBits && _curChannel >= 6) {
                return;
            }

            // This means the "Key On" bit will always be 0
            channel.regBx &= 0xDF;

            // Octave / F-Number / Key-On
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        void ADLDriver::initAdlibChannel(const uint8_t channel)
        {
            //debugC(9, kDebugLevelSound, "initAdlibChannel(%d)", chan);
            spdlog::debug("initAdlibChannel(%d)", channel);

            // The control channel has no corresponding AdLib channel

            if (channel >= NUM_CHANNELS) {
                return;
            }

            // I believe this has to do with channels 6, 7, and 8 being special
            // when AdLib's rhythm section is enabled.

            if (_rhythmSectionBits && channel >= 6) {
                return;
            }

            uint8_t offset = _regOffset[channel];

            // The channel is cleared: First the attack/delay rate,
            writeOPL(0x60 + offset, 0xFF);
            writeOPL(0x63 + offset, 0xFF);
            // then the sustain level/release rate,
            writeOPL(0x80 + offset, 0xFF);
            writeOPL(0x83 + offset, 0xFF);
            // and finally the note is turned off.
            writeOPL(0xB0 + channel, 0x00);

            // ...and then the note is turned on again, with whatever value is
            // still lurking in the A0 + chan register, but everything else -
            // including the two most significant frequency bit, and the octave -
            // set to zero.
            //
            // This is very strange behavior, and causes problems with the ancient
            // FMOPL code we borrowed from AdPlug. I've added a workaround. See
            // audio/softsynth/opl/mame.cpp for more details.
            //
            // Fortunately, the more modern DOSBox FMOPL code does not seem to have
            // any trouble with this.
            writeOPL(0xB0 + channel, 0x20);
        }

        uint16_t ADLDriver::getRandomNr()
        {
            _rnd += RANDOM_INC;
            uint16_t lowBits = _rnd & 7;
            _rnd >>= 3;
            _rnd |= (lowBits << 13);

            return _rnd;
        }

        void ADLDriver::setupDuration(const uint8_t duration, Channel& channel)
        {
            spdlog::debug("setupDuration(%d, %lu)", duration, static_cast<long>(&channel - _channels));

            if (channel.durationRandomness) {
                channel.duration = duration + (getRandomNr() & channel.durationRandomness);
                return;
            }

            if (channel.fractionalSpacing) {
                channel.spacing2 = (duration >> 3) * channel.fractionalSpacing;
            }

            channel.duration = duration;
        }

        /// <summary>
        /// This function may or may not play the note. It's usually followed by a call
        /// to noteOn(), which will always play the current note.
        /// </summary>
        /// <param name="rawNote"></param>
        /// <param name="channel"></param>
        /// <param name="flag"></param>
        void ADLDriver::setupNote(const uint8_t rawNote, Channel& channel, const bool flag)
        {
            //debugC(9, kDebugLevelSound, "setupNote(%d, %lu)", rawNote, (long)(&channel - _channels));
            spdlog::debug("setupNote(%d, %lu)", rawNote, static_cast<long>(&channel - _channels));
            
            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            channel.rawNote = rawNote;

            int8_t note = (rawNote & 0x0F) + channel.baseNote;
            int8_t octave = ((rawNote + channel.baseOctave) >> 4) & 0x0F;

            // There are only twelve notes. If we go outside that, we have to
            // adjust the note and octave.
            if (note >= 12) {
                octave += note / 12;
                note %= 12;
            }
            else if (note < 0) {
                int8_t octaves = -(note + 1) / 12 + 1;
                octave -= octaves;
                note += 12 * octaves;
            }

            // The calculation of frequency looks quite different from the original
            // disassembly at a first glance, but when you consider that the
            // largest possible value would be 0x0246 + 0xFF + 0x47 (and that's if
            // baseFreq is unsigned), freq is still a 10-bit value, just as it
            // should be to fit in the Ax and Bx registers.
            //
            // If it were larger than that, it could have overflowed into the
            // octave bits, and that could possibly have been used in some sound.
            // But as it is now, I can't see any way it would happen.
            uint16_t freq = _freqTable[note] + channel.baseFreq;

            // When called from callback 41, the behavior is slightly different:
            // We adjust the frequency, even when channel.pitchBend is 0.
            if (channel.pitchBend || flag)
            {
                const uint8_t* table;
                // For safety, limit the values used to index the tables.
                uint8_t indexNote = CLIP(rawNote & 0x0F, 0, 11);

                if (channel.pitchBend >= 0) {
                    table = _pitchBendTables[indexNote + 2];
                    freq += table[CLIP(+channel.pitchBend, 0, 31)];
                }
                else {
                    table = _pitchBendTables[indexNote];
                    freq -= table[CLIP(-channel.pitchBend, 0, 31)];
                }
            }

            // Shift octave to correct bit position and limit to valid range.
            octave = CLIP<int8_t>(octave, 0, 7) << 2;

            // Update octave & frequency, but keep on/off state.
            channel.regAx = freq & 0xFF;
            channel.regBx = (channel.regBx & 0x20) | octave | ((freq >> 8) & 0x03);

            writeOPL(0xA0 + _curChannel, channel.regAx);
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        void ADLDriver::setupInstrument(uint8_t regOffset, const uint8_t* dataptr, Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "setupInstrument(%d, %p, %lu)", regOffset, (const void*)dataptr, (long)(&channel - _channels));
            spdlog::debug("setupInstrument(%d, %p, %lu)", regOffset, static_cast<const void*>(dataptr), static_cast<long>(&channel - _channels));

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            // Safety check: need 11 bytes of data.
            if (!checkDataOffset(dataptr, 11)) {
                return;
            }

            // Amplitude Modulation / Vibrato / Envelope Generator Type /
            // Keyboard Scaling Rate / Modulator Frequency Multiple
            writeOPL(0x20 + regOffset, *dataptr++);
            writeOPL(0x23 + regOffset, *dataptr++);

            uint8_t temp = *dataptr++;

            // Feedback / Algorithm

            // It is very likely that _curChannel really does refer to the same
            // channel as regOffset, but there's only one Cx register per channel.
            writeOPL(0xC0 + _curChannel, temp);

            // The algorithm bit. I don't pretend to understand this fully, but
            // "If set to 0, operator 1 modulates operator 2. In this case,
            // operator 2 is the only one producing sound. If set to 1, both
            // operators produce sound directly. Complex sounds are more easily
            // created if the algorithm is set to 0."
            channel.twoChan = temp & 1;

            // Waveform Select
            writeOPL(0xE0 + regOffset, *dataptr++);
            writeOPL(0xE3 + regOffset, *dataptr++);

            channel.opLevel1 = *dataptr++;
            channel.opLevel2 = *dataptr++;

            // Level Key Scaling / Total Level
            writeOPL(0x40 + regOffset, calculateOpLevel1(channel));
            writeOPL(0x43 + regOffset, calculateOpLevel2(channel));

            // Attack Rate / Decay Rate
            writeOPL(0x60 + regOffset, *dataptr++);
            writeOPL(0x63 + regOffset, *dataptr++);

            // Sustain Level / Release Rate
            writeOPL(0x80 + regOffset, *dataptr++);
            writeOPL(0x83 + regOffset, *dataptr++);
        }

        void ADLDriver::noteOn(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "noteOn(%lu)", (long)(&channel - _channels));
            spdlog::debug("noteOn(%lu)", static_cast<long>(&channel - _channels));

            // The "note on" bit is set, and the current note is played.
            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            channel.regBx |= 0x20;
            writeOPL(0xB0 + _curChannel, channel.regBx);

            // Update vibrato effect variables: vibratoStep is set to a
            // vibratoStepRange+1-bit value proportional to the note's f-number.
            // Reinitalize delay countdown; vibratoStepsCountdown reinitialization omitted.
            int8_t shift = 9 - CLIP<int8_t>(channel.vibratoStepRange, 0, 9);
            uint16_t freq = ((channel.regBx << 8) | channel.regAx) & 0x3FF;
            channel.vibratoStep = (freq >> shift) & 0xFF;
            channel.vibratoDelayCountdown = channel.vibratoDelay;
        }

        void ADLDriver::adjustVolume(Channel& channel)
        {
            spdlog::debug("adjustVolume(%lu)", static_cast<long>(&channel - _channels));

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            // Level Key Scaling / Total Level
            writeOPL(0x43 + _regOffset[_curChannel], calculateOpLevel2(channel));
            if (channel.twoChan) {
                writeOPL(0x40 + _regOffset[_curChannel], calculateOpLevel1(channel));
            }
        }

        /// <summary>
        /// This is presumably only used for some sound effects, e.g. Malcolm blowing up
        /// the trees in the intro (but not the effect where he "booby-traps" the big
        /// tree) and turning Kallak to stone. Related functions and variables:
        ///
        /// update_setupPrimaryEffectSlide()
        ///    - Initializes slideTempo, slideStep and slideTimer
        ///    - slideTempo is not further modified
        ///    - slideStep is not further modified, except by update_removePrimaryEffectSlide()
        ///
        /// update_removePrimaryEffectSlide()
        ///    - Deinitializes slideStep
        ///
        /// slideTempo - determines how often the frequency is updated
        /// slideStep  - amount the frequency changes each update
        /// slideTimer - keeps track of time
        /// </summary>
        /// <param name="channel"></param>
        void ADLDriver::primaryEffectSlide(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "Calling primaryEffectSlide (channel: %d)", _curChannel);
            spdlog::debug("Calling primaryEffectSlide (channel: %d)", _curChannel);

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            // Time for next frequency update?
            if (!advance(channel.slideTimer, channel.slideTempo)) {
                return;
            }

            // Extract current frequency, (shifted) octave, and "note on" bit into
            // separate variable so calculations can't overflow into other fields.
            int16_t freq = ((channel.regBx & 0x03) << 8) | channel.regAx;
            uint8_t octave = channel.regBx & 0x1C;
            uint8_t note_on = channel.regBx & 0x20;

            // Limit slideStep to prevent integer overflow.
            freq += CLIP<int16_t>(channel.slideStep, -0x3FF, 0x3FF);

            if (channel.slideStep >= 0 && freq >= 734)
            {
                // The new frequency is too high. Shift it down and go
                // up one octave.
                freq >>= 1;
                if (!(freq & 0x3FF)) {
                    ++freq;
                }
                octave += 4;
            }
            else if (channel.slideStep < 0 && freq < 388)
            {
                // Safety check: a negative frequency triggers undefined
                // behavior for the left shift operator below.
                if (freq < 0) {
                    freq = 0;
                }

                // The new frequency is too low. Shift it up and go
                // down one octave.
                freq <<= 1;
                if (!(freq & 0x3FF)) {
                    --freq;
                }
                octave -= 4;
            }

            // Set new frequency and octave.
            channel.regAx = freq & 0xFF;
            channel.regBx = note_on | (octave & 0x1C) | ((freq >> 8) & 0x03);

            writeOPL(0xA0 + _curChannel, channel.regAx);
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        /// <summary>
        /// This is presumably only used for some sound effects, e.g. Malcolm entering
        /// and leaving Kallak's hut. Related functions and variables:
        ///
        /// update_setupPrimaryEffectVibrato()
        ///    - Initializes vibratoTempo, vibratoStepRange, vibratoStepsCountdown,
        ///      vibratoNumSteps, and vibratoDelay
        ///    - vibratoTempo is not further modified
        ///    - vibratoStepRange is not further modified
        ///    - vibratoStepsCountdown is a countdown that gets reinitialized to
        ///      vibratoNumSteps on zero, but is initially only half as much
        ///    - vibratoNumSteps is not further modified
        ///    - vibratoDelay is not further modified
        ///
        /// noteOn()
        ///    - Plays the current note
        ///    - Sets vibratoStep depending on vibratoStepRange and the note's f-number
        ///    - Initializes vibratoDelayCountdown with vibratoDelay
        ///
        /// vibratoTempo          - determines how often the frequency is updated
        /// vibratoStepRange      - determines frequency step size depending on f-number
        /// vibratoStepsCountdown - reverses slide direction on zero
        /// vibratoNumSteps       - initializer for vibratoStepsCountdown countdown
        /// vibratoDelay          - initializer for vibratoDelayCountdown
        /// vibratoStep           - amount the frequency changes each update
        /// vibratoDelayCountdown - effect starts when it reaches zero
        /// vibratoTimer          - keeps track of time
        ///
        /// Note that vibratoTimer is never initialized. Not that it should matter much,
        /// but it is a bit sloppy. Also vibratoStepsCountdown should be reset to its
        /// initial value in noteOn() but isn't.
        /// </summary>
        /// <param name="channel"></param>
        void ADLDriver::primaryEffectVibrato(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "Calling primaryEffectVibrato (channel: %d)", _curChannel);
            spdlog::debug("Calling primaryEffectVibrato (channel: %d)", _curChannel);
            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            // When a new note is played the effect doesn't start immediately.
            if (channel.vibratoDelayCountdown) {
                --channel.vibratoDelayCountdown;
                return;
            }

            // Time for an update?
            if (advance(channel.vibratoTimer, channel.vibratoTempo))
            {
                // Reverse direction every vibratoNumSteps updates
                if (!(--channel.vibratoStepsCountdown)) {
                    channel.vibratoStep = -channel.vibratoStep;
                    channel.vibratoStepsCountdown = channel.vibratoNumSteps;
                }

                // Update frequency.
                uint16_t freq = ((channel.regBx << 8) | channel.regAx) & 0x3FF;
                freq += channel.vibratoStep;

                channel.regAx = freq & 0xFF;
                channel.regBx = (channel.regBx & 0xFC) | (freq >> 8);

                // Octave / F-Number / Key-On
                writeOPL(0xA0 + _curChannel, channel.regAx);
                writeOPL(0xB0 + _curChannel, channel.regBx);
            }
        }

        /// <summary>
        /// I don't know where this is used. An OPL register is regularly updated
        /// with data from a chunk of the _soundData[] buffer, i.e., one instrument
        /// parameter register is modulated with data from the chunk. The data is
        /// reused repeatedly starting from the end of the chunk.
        ///
        /// Since we use _curRegOffset to specify the final register, it's quite
        /// unlikely that this function is ever used to play notes. It's probably only
        /// used to modify the sound. Another thing that supports this idea is that it
        /// can be combined with any of the effects callbacks above.
        ///
        /// Related functions and variables:
        ///
        /// update_setupSecondaryEffect1()
        ///    - Initialies secondaryEffectTimer, secondaryEffectTempo,
        ///      secondaryEffectSize, secondaryEffectPos, secondaryEffectRegbase,
        ///      and secondaryEffectData
        ///    - secondaryEffectTempo is not further modified
        ///    - secondaryEffectSize is not further modified
        ///    - secondaryEffectRegbase is not further modified
        ///    - secondaryEffectData is not further modified
        ///
        /// secondaryEffectTimer   - keeps track of time
        /// secondaryEffectTempo   - determines how often the operation is performed
        /// secondaryEffectSize    - the size of the data chunk
        /// secondaryEffectPos     - the current index into the data chunk
        /// secondaryEffectRegbase - the operation to perform
        /// secondaryEffectData    - the offset of the data chunk
        /// </summary>
        /// <param name="channel"></param>
        void ADLDriver::secondaryEffect1(Channel& channel)
        {
            //debugC(9, kDebugLevelSound, "Calling secondaryEffect1 (channel: %d)", _curChannel);
            spdlog::debug("Calling secondaryEffect1 (channel: %d)", _curChannel);

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            if (advance(channel.secondaryEffectTimer, channel.secondaryEffectTempo))
            {
                if (--channel.secondaryEffectPos < 0) {
                    channel.secondaryEffectPos = channel.secondaryEffectSize;
                }

                writeOPL(
                    channel.secondaryEffectRegbase + _curRegOffset,
                    _soundData[channel.secondaryEffectData + channel.secondaryEffectPos]
                );
            }
        }

        uint8_t ADLDriver::calculateOpLevel1(Channel& channel)
        {
            uint8_t value = channel.opLevel1 & 0x3F;

            if (channel.twoChan) {
                value += channel.opExtraLevel1;
                value += channel.opExtraLevel2;

                uint16_t level3 = (channel.opExtraLevel3 ^ 0x3F) * channel.volumeModifier;
                if (level3) {
                    level3 += 0x3F;
                    level3 >>= 8;
                }

                value += level3 ^ 0x3F;
            }

            // The clipping as signed instead of unsigned caused very ugly noises in LOK when the music
            // was fading out in certain situations (bug #11303). The bug seems to come to surface only
            // when the volume is not set to the maximum.
            // I have confirmed that the noise bug also appears in LOL floppy (Westwood logo sound). It has
            // been reported to be present in EOB 1 (intro music), but I haven't been able to confirm it.
            // The original AdLib drivers all do the same wrong clipping. At least in the original EOB and
            // LOK games this wouldn't cause issues, since the original drivers (and games) do not have
            // volume settings and use a simpler calculation of the total level (just adding the three
            // opExtraLevels to the opLevel).
            // The later (HOF/LOL) original drivers do the same wrong clipping, too. But original LOL floppy
            // doesn't have volume settings either. And with max volume the logo sound is okay...
            if (value & 0x80) {
                //debugC(3, kDebugLevelSound, "AdLibDriver::calculateOpLevel1(): WORKAROUND - total level clipping uint/int bug encountered");
                spdlog::debug("AdLibDriver::calculateOpLevel1(): WORKAROUND - total level clipping uint/int bug encountered");
            }

            value = CLIP<uint8_t>(value, 0, 0x3F);

            if (!channel.volumeModifier) {
                value = 0x3F;
            }

            // Preserve the scaling level bits from opLevel1
            return value | (channel.opLevel1 & 0xC0);
        }

        uint8_t ADLDriver::calculateOpLevel2(Channel& channel)
        {
            uint8_t value = channel.opLevel2 & 0x3F;

            value += channel.opExtraLevel1;
            value += channel.opExtraLevel2;

            uint16_t level3 = (channel.opExtraLevel3 ^ 0x3F) * channel.volumeModifier;
            if (level3) {
                level3 += 0x3F;
                level3 >>= 8;
            }

            value += level3 ^ 0x3F;

            // See comment in calculateOpLevel1()
            if (value & 0x80) {
                //debugC(3, kDebugLevelSound, "AdLibDriver::calculateOpLevel2(): WORKAROUND - total level clipping uint/int bug encountered");
                spdlog::debug("AdLibDriver::calculateOpLevel2(): WORKAROUND - total level clipping uint/int bug encountered");
            }

            value = CLIP<uint8_t>(value, 0, 0x3F);

            if (!channel.volumeModifier) {
                value = 0x3F;
            }

            // Preserve the scaling level bits from opLevel2
            return value | (channel.opLevel2 & 0xC0);
        }

        uint16_t ADLDriver::checkValue(const int16_t val)
        {
            return CLIP<int16_t>(val, 0, 0x3F);
        }

        bool ADLDriver::advance(uint8_t& timer, const uint8_t tempo)
        {
            uint8_t old = timer;
            timer += tempo;

            return timer < old;
        }

        const uint8_t* ADLDriver::checkDataOffset(const uint8_t* ptr, const long n)
        {
            if (ptr)
            {
                long offset = ptr - _soundData;

                if (n >= -offset && n <= (long)_soundDataSize - offset) {
                    return ptr + n;
                }
            }

            return nullptr;
        }

        uint8_t* ADLDriver::getProgram(int progId)
        {
            //TODO: move in the ADLFile

            // Safety check: invalid progId would crash.
            if (progId < 0 || progId >= (int32_t)_soundDataSize / 2)
                return nullptr;

            const uint16_t offset = READ_LE_UINT16(_soundData + 2 * progId);

            // In case an invalid offset is specified we return nullptr to
            // indicate an error. 0xFFFF seems to indicate "this is not a valid
            // program/instrument". However, 0 is also invalid because it points
            // inside the offset table itself. We also ignore any offsets outside
            // of the actual data size.
            // The original does not contain any safety checks and will simply
            // read outside of the valid sound data in case an invalid offset is
            // encountered.
            if (offset == 0 || offset >= _soundDataSize) {
                return nullptr;
            }
            else {
                return _soundData + offset;
            }
        }

        const uint8_t* ADLDriver::getInstrument(int instrumentId)
        {
            // TODO: Move in ADLFile

            return getProgram(_numPrograms + instrumentId);
        }



        int ADLDriver::update_setRepeat(Channel& channel, const uint8_t* values)
        {

        }

        int ADLDriver::update_checkRepeat(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupProgram(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setNoteSpacing(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_jump(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_jumpToSubroutine(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_returnFromSubroutine(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setBaseOctave(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_stopChannel(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_playRest(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_writeAdLib(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupNoteAndDuration(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setBaseNote(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupSecondaryEffect1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_stopOtherChannel(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_waitForEndOfProgram(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupInstrument(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupPrimaryEffectSlide(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_removePrimaryEffectSlide(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setBaseFreq(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupPrimaryEffectVibrato(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setPriority(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setBeat(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_waitForNextBeat(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setExtraLevel1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupDuration(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_playNote(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setFractionalNoteSpacing(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setTempo(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_removeSecondaryEffect1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setChannelTempo(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setExtraLevel3(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setExtraLevel2(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_changeExtraLevel2(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setAMDepth(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setVibratoDepth(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_changeExtraLevel1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_clearChannel(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_changeNoteRandomly(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_removePrimaryEffectVibrato(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_pitchBend(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_resetToGlobalTempo(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_nop(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setDurationRandomness(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_changeChannelTempo(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::updateCallback46(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setupRhythmSection(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_playRhythmSection(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_removeRhythmSection(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setRhythmLevel2(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_changeRhythmLevel1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setRhythmLevel1(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setSoundTrigger(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::update_setTempoReset(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

        int ADLDriver::updateCallback56(Channel& channel, const uint8_t* values)
        {
            return 0;
        }

}
}
