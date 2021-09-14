#include <drivers/westwood/ADLDriver.hpp>
#include <spdlog/spdlog.h>
#include <functional>
#include <utils/algorithms.hpp>
#include <utils/endianness.hpp>
#include <algorithm>

using utils::CLIP;
using utils::READ_BE_UINT16;
using utils::READ_LE_UINT16;

constexpr int CALLBACKS_PER_SECOND = 72;

constexpr int NUM_CHANNELS = 9;
constexpr int RANDOM_SEED = 0x1234;
constexpr int RANDOM_INC = 0x9248;

// TODO: review it / remove / replace / refactor
#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

namespace drivers
{
    namespace westwood
    {
        ADLDriver::ADLDriver(std::shared_ptr<hardware::opl::OPL> opl, std::shared_ptr<files::westwood::ADLFile> adl_file)
            : _opl(opl)
        {
            if (!_opl || !_opl->init()) {
                spdlog::error("Failed to initialize OPL");
            }

            memset(_channels, 0, sizeof(_channels));

            _vibratoAndAMDepthBits = _curRegOffset = 0;

            _curChannel = _rhythmSectionBits = 0;
            _rnd = RANDOM_SEED;

            _tempo = 0;
            _soundTrigger = 0;
            _programStartTimeout = 0;

            _callbackTimer = 0xFF;
            _beatDivider = _beatDivCnt = _beatCounter = _beatWaiting = 0;
            _opLevelBD = _opLevelHH = _opLevelSD = _opLevelTT = _opLevelCY = 0;
            _opExtraLevel1HH = _opExtraLevel2HH =
            _opExtraLevel1CY = _opExtraLevel2CY =
            _opExtraLevel2TT = _opExtraLevel1TT =
            _opExtraLevel1SD = _opExtraLevel2SD =
            _opExtraLevel1BD = _opExtraLevel2BD = 0;

            _tablePtr1 = _tablePtr2 = nullptr;

            _syncJumpMask = 0;

            _musicVolume = 0;
            _sfxVolume = 0;

            _sfxPointer = nullptr;

            _programQueueStart = _programQueueEnd = 0;
            _retrySounds = false;

            hardware::opl::TimerCallBack cb = std::bind(&ADLDriver::callback, this);
            auto p = std::make_shared<hardware::opl::TimerCallBack>(cb);
            _opl->start(p, CALLBACKS_PER_SECOND);

            stopAllChannels();
            setADLFile(adl_file);
            initDriver();
            setMusicVolume(255);
            setSfxVolume(255);
        }

        ADLDriver::~ADLDriver()
        {
        }

        void ADLDriver::setADLFile(const std::shared_ptr<files::westwood::ADLFile> adl_file) noexcept
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            _adl_file = adl_file;
            _version = _adl_file->getVersion();
            _soundDataSize = _adl_file->getDataSize();
            _soundData = _adl_file->getData();

            // Drop all tracks that are still queued. These would point to the old
            // sound data.
            _programQueueStart = _programQueueEnd = 0;
            for (int i = 0; i < ARRAYSIZE(_programQueue); ++i)
                _programQueue[i] = QueueEntry();

            _sfxPointer = nullptr;
        }

        void ADLDriver::initDriver()
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            resetAdLibState();
        }

        void ADLDriver::startSound(const int track, const int volume)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            uint8_t* trackData = getProgram(track);
            spdlog::debug("getProgma[track={}]= {0:d}", track, trackData);
            if (!trackData) {
                return;
            }

            // We used to drop the new sound here, but that isn't the behavior of the original code.
            // It would cause more issues than do any good. Now, we just have a debug message and
            // then drop the oldest sound, like the original driver...
            if (_programQueueEnd == _programQueueStart && _programQueue[_programQueueEnd].data != 0) {
                spdlog::debug("ADLDriver: Program queue full, dropping track {}", _programQueue[_programQueueEnd].id);
            }

            _programQueue[_programQueueEnd] = QueueEntry(trackData, track, volume);
            ++_programQueueEnd &= 15;
        }

        bool ADLDriver::isChannelPlaying(const int channel)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            assert(channel >= 0 && channel <= NUM_CHANNELS);
            return (_channels[channel].dataptr != nullptr);
        }

        void ADLDriver::stopAllChannels()
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            for (int channel = 0; channel <= 9; ++channel) {
                _curChannel = channel;

                Channel& chan = _channels[_curChannel];
                chan.priority = 0;
                chan.dataptr = 0;

                if (channel != NUM_CHANNELS) {
                    noteOff(chan);
                }
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

        // timer callback
        //
        // Starts and executes programs and maintains a global beat that channels
        // can synchronize on.
        void ADLDriver::callback()
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            if (_programStartTimeout)
                --_programStartTimeout;
            else
                setupPrograms();
            executePrograms();

            if (advance(_callbackTimer, _tempo)) {
                if (!(--_beatDivCnt)) {
                    _beatDivCnt = _beatDivider;
                    ++_beatCounter;
                }
            }
        }

        void ADLDriver::setSyncJumpMask(const uint16_t mask)
        {
            _syncJumpMask = mask;
        }

        void ADLDriver::setMusicVolume(const uint8_t volume)
        {
            const std::lock_guard<std::mutex> lock(_mutex);

            _musicVolume = volume;

            for (uint8_t i = 0; i < 6; ++i) {
                Channel& chan = _channels[i];
                chan.volumeModifier = volume;

                const int8_t regOffset = _regOffset[i];

                // Level Key Scaling / Total Level
                writeOPL(0x40 + regOffset, calculateOpLevel1(chan));
                writeOPL(0x43 + regOffset, calculateOpLevel2(chan));
            }

            // For now we use the music volume for both sfx and music in Kyra1 and EoB
            //if (_version < 4)
            if (_version < 3)
            {
                _sfxVolume = volume;

                for (uint8_t i = 6; i < NUM_CHANNELS; ++i)
                {
                    Channel& chan = _channels[i];
                    chan.volumeModifier = volume;

                    const int8_t regOffset = _regOffset[i];

                    // Level Key Scaling / Total Level
                    writeOPL(0x40 + regOffset, calculateOpLevel1(chan));
                    writeOPL(0x43 + regOffset, calculateOpLevel2(chan));
                }
            }
        }

        void ADLDriver::setSfxVolume(const uint8_t volume)
        {
            // We only support sfx volume in version 4 games.
            if (_version < 3)
                return;

            const std::lock_guard<std::mutex> lock(_mutex);

            _sfxVolume = volume;

            for (uint8_t i = 6; i < 9; ++i) {
                Channel& chan = _channels[i];
                chan.volumeModifier = volume;

                const int8_t regOffset = _regOffset[i];

                // Level Key Scaling / Total Level
                writeOPL(0x40 + regOffset, calculateOpLevel1(chan));
                writeOPL(0x43 + regOffset, calculateOpLevel2(chan));
            }
        }

        void ADLDriver::play(const uint8_t track, const uint8_t volume)
        {
            uint16_t soundId = 0;

            soundId = _adl_file->getTrack(track);

            if ((soundId == 0xFFFF && _version == 3) || (soundId == 0xFF && _version < 3) || _soundData == nullptr)
                return;

            spdlog::debug("trackEntries[track = {}] = {}", track, soundId);
            startSound(soundId, volume);
        }

        bool ADLDriver::isPlaying()
        {
            return isChannelPlaying(0);
        }

        uint8_t* ADLDriver::getProgram(const int progId, const files::westwood::ADLFile::PROG_TYPE progType)
        {
            if (_adl_file == nullptr) {
                spdlog::error("ADLDriver::getProgram(): no ADL file loaded.");
                return nullptr;
            }

            const uint16_t offset = _adl_file->getProgramOffset(progId, progType);
            spdlog::debug("calling getProgram(prodIg={}){}", progId, offset);

            // In case an invalid offset is specified we return nullptr to
            // indicate an error. 0xFFFF seems to indicate "this is not a valid
            // program/instrument". However, 0 is also invalid because it points
            // inside the offset table itself. We also ignore any offsets outside
            // of the actual data size.
            // The original does not contain any safety checks and will simply
            // read outside of the valid sound data in case an invalid offset is
            // encountered.
            // offset = 0 is valid now as offset are adjusted when read the file
            if (offset >= _soundDataSize) {
                spdlog::warn("ADLDriver::getProgram(): invalid offset read. offset={} --- _soundDataSize={}", offset, _soundDataSize);
                return nullptr;
            }

            return _soundData.get() + offset;
        }

        uint8_t* ADLDriver::getProgram(const int progId)
        {
            return getProgram(progId, files::westwood::ADLFile::PROG_TYPE::TRACK);
        }

        const uint8_t* ADLDriver::getInstrument(const int instrumentId)
        {
            return getProgram(instrumentId, files::westwood::ADLFile::PROG_TYPE::INSTRUMENT);
        }

        // This is presumably only used for some sound effects, e.g. Malcolm blowing up
        // the trees in the intro (but not the effect where he "booby-traps" the big
        // tree) and turning Kallak to stone. Related functions and variables:
        //
        // update_setupPrimaryEffectSlide()
        //    - Initializes slideTempo, slideStep and slideTimer
        //    - slideTempo is not further modified
        //    - slideStep is not further modified, except by update_removePrimaryEffectSlide()
        //
        // update_removePrimaryEffectSlide()
        //    - Deinitializes slideStep
        //
        // slideTempo - determines how often the frequency is updated
        // slideStep  - amount the frequency changes each update
        // slideTimer - keeps track of time
        void ADLDriver::primaryEffectSlide(Channel& channel)
        {
            spdlog::debug("Calling primaryEffectSlide (channel: {})", _curChannel);

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
            int8_t octave = channel.regBx & 0x1C;
            int8_t note_on = channel.regBx & 0x20;

            // Limit slideStep to prevent integer overflow.
            freq += CLIP<int16_t>(channel.slideStep, -0x3FF, 0x3FF);

            if (channel.slideStep >= 0 && freq >= 734)
            {
                // The new frequency is too high. Shift it down and go
                // up one octave.
                freq >>= 1;
                if (!(freq & 0x3FF))
                    ++freq;
                octave += 4;
            }
            else if (channel.slideStep < 0 && freq < 388)
            {
                // Safety check: a negative frequency triggers undefined
                // behavior for the left shift operator below.
                if (freq < 0)
                    freq = 0;

                // The new frequency is too low. Shift it up and go
                // down one octave.
                freq <<= 1;
                if (!(freq & 0x3FF))
                    --freq;
                octave -= 4;
            }

            // Set new frequency and octave.
            channel.regAx = freq & 0xFF;
            channel.regBx = note_on | (octave & 0x1C) | ((freq >> 8) & 0x03);

            writeOPL(0xA0 + _curChannel, channel.regAx);
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        // This is presumably only used for some sound effects, e.g. Malcolm entering
        // and leaving Kallak's hut. Related functions and variables:
        //
        // update_setupPrimaryEffectVibrato()
        //    - Initializes vibratoTempo, vibratoStepRange, vibratoStepsCountdown,
        //      vibratoNumSteps, and vibratoDelay
        //    - vibratoTempo is not further modified
        //    - vibratoStepRange is not further modified
        //    - vibratoStepsCountdown is a countdown that gets reinitialized to
        //      vibratoNumSteps on zero, but is initially only half as much
        //    - vibratoNumSteps is not further modified
        //    - vibratoDelay is not further modified
        //
        // noteOn()
        //    - Plays the current note
        //    - Sets vibratoStep depending on vibratoStepRange and the note's f-number
        //    - Initializes vibratoDelayCountdown with vibratoDelay
        //
        // vibratoTempo          - determines how often the frequency is updated
        // vibratoStepRange      - determines frequency step size depending on f-number
        // vibratoStepsCountdown - reverses slide direction on zero
        // vibratoNumSteps       - initializer for vibratoStepsCountdown countdown
        // vibratoDelay          - initializer for vibratoDelayCountdown
        // vibratoStep           - amount the frequency changes each update
        // vibratoDelayCountdown - effect starts when it reaches zero
        // vibratoTimer          - keeps track of time
        //
        // Note that vibratoTimer is never initialized. Not that it should matter much,
        // but it is a bit sloppy. Also vibratoStepsCountdown should be reset to its
        // initial value in noteOn() but isn't.
        void ADLDriver::primaryEffectVibrato(Channel& channel)
        {
            spdlog::debug("Calling primaryEffectVibrato (channel: {})", _curChannel);

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

        // I don't know where this is used. An OPL register is regularly updated
        // with data from a chunk of the _soundData[] buffer, i.e., one instrument
        // parameter register is modulated with data from the chunk. The data is
        // reused repeatedly starting from the end of the chunk.
        //
        // Since we use _curRegOffset to specify the final register, it's quite
        // unlikely that this function is ever used to play notes. It's probably only
        // used to modify the sound. Another thing that supports this idea is that it
        // can be combined with any of the effects callbacks above.
        //
        // Related functions and variables:
        //
        // update_setupSecondaryEffect1()
        //    - Initialies secondaryEffectTimer, secondaryEffectTempo,
        //      secondaryEffectSize, secondaryEffectPos, secondaryEffectRegbase,
        //      and secondaryEffectData
        //    - secondaryEffectTempo is not further modified
        //    - secondaryEffectSize is not further modified
        //    - secondaryEffectRegbase is not further modified
        //    - secondaryEffectData is not further modified
        //
        // secondaryEffectTimer   - keeps track of time
        // secondaryEffectTempo   - determines how often the operation is performed
        // secondaryEffectSize    - the size of the data chunk
        // secondaryEffectPos     - the current index into the data chunk
        // secondaryEffectRegbase - the operation to perform
        // secondaryEffectData    - the offset of the data chunk
        void ADLDriver::secondaryEffect1(Channel& channel) {
            spdlog::debug("Calling secondaryEffect1 (channel: {})", _curChannel);

            if (_curChannel >= NUM_CHANNELS) {
                return;
            }

            if (advance(channel.secondaryEffectTimer, channel.secondaryEffectTempo))
            {
                if (--channel.secondaryEffectPos < 0) {
                    channel.secondaryEffectPos = channel.secondaryEffectSize;
                }

                writeOPL(channel.secondaryEffectRegbase + _curRegOffset,
                    _soundData[channel.secondaryEffectData + channel.secondaryEffectPos]);
            }
        }
        
        void ADLDriver::adjustSfxData(uint8_t* ptr, int volume) {
            // Check whether we need to reset the data of an old sfx which has been
            // started.
            if (_sfxPointer) {
                _sfxPointer[1] = _sfxPriority;
                _sfxPointer[3] = _sfxVelocity;
                _sfxPointer = nullptr;
            }

            // Only music tracks are started on channel 9, thus we need to make sure
            // we do not have a music track here.
            if (*ptr == 9)
                return;

            // Store the pointer so we can reset the data when a new program is started.
            _sfxPointer = ptr;

            // Store the old values.
            _sfxPriority = ptr[1];
            _sfxVelocity = ptr[3];

            // Adjust the values.
            if (volume != 0xFF) {
                if (_version >= 3) {
                    int newVal = ((((ptr[3]) + 63) * volume) >> 8) & 0xFF;
                    ptr[3] = -newVal + 63;
                    ptr[1] = ((ptr[1] * volume) >> 8) & 0xFF;
                }
                else {
                    int newVal = ((_sfxVelocity << 2) ^ 0xFF) * volume;
                    ptr[3] = (newVal >> 10) ^ 0x3F;
                    ptr[1] = newVal >> 11;
                }
            }
        }

        void ADLDriver::resetAdLibState()
        {
            spdlog::debug("resetAdLibState()");

            _rnd = 0x1234;

            // Authorize the control of the waveforms
            writeOPL(0x01, 0x20);

            // Select FM music mode
            writeOPL(0x08, 0x00);

            // I would guess the main purpose of this is to turn off the rhythm,
            // thus allowing us to use 9 melodic voices instead of 6.
            writeOPL(0xBD, 0x00);

            initChannel(_channels[NUM_CHANNELS]);
            for (int loop = 8; loop >= 0; loop--) {
                // Silence the channel
                writeOPL(0x40 + _regOffset[loop], 0x3F);
                writeOPL(0x43 + _regOffset[loop], 0x3F);
                initChannel(_channels[loop]);
            }
        }

        void ADLDriver::writeOPL(uint8_t reg, uint8_t val) {
            _opl->writeReg(reg, val);
        }

        void ADLDriver::initChannel(Channel& channel) {
            spdlog::debug("initChannel({})", (long)(&channel - _channels));

            int8_t backupEL2 = channel.opExtraLevel2;
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
            spdlog::debug("noteOff({})", (long)(&channel - _channels));

            // The control channel has no corresponding AdLib channel

            if (_curChannel >= NUM_CHANNELS)
                return;

            // When the rhythm section is enabled, channels 6, 7 and 8 are special.

            if (_rhythmSectionBits && _curChannel >= 6)
                return;

            // This means the "Key On" bit will always be 0
            channel.regBx &= 0xDF;

            // Octave / F-Number / Key-On
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        void ADLDriver::initAdlibChannel(uint8_t chan)
        {
            spdlog::debug("initAdlibChannel({})", chan);

            // The control channel has no corresponding AdLib channel

            if (chan >= NUM_CHANNELS)
                return;

            // I believe this has to do with channels 6, 7, and 8 being special
            // when AdLib's rhythm section is enabled.

            if (_rhythmSectionBits && chan >= 6)
                return;

            int8_t offset = _regOffset[chan];

            // The channel is cleared: First the attack/delay rate, then the
            // sustain level/release rate, and finally the note is turned off.

            writeOPL(0x60 + offset, 0xFF);
            writeOPL(0x63 + offset, 0xFF);

            writeOPL(0x80 + offset, 0xFF);
            writeOPL(0x83 + offset, 0xFF);

            writeOPL(0xB0 + chan, 0x00);

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

            writeOPL(0xB0 + chan, 0x20);
        }

        // I believe this is a random number generator. It actually does seem to
        // generate an even distribution of almost all numbers from 0 through 65535,
        // though in my tests some numbers were never generated.
        uint16_t ADLDriver::getRandomNr()
        {
            _rnd += RANDOM_INC;
            uint16_t lowBits = _rnd & 7;
            _rnd >>= 3;
            _rnd |= (lowBits << 13);
            return _rnd;
        }

        void ADLDriver::setupDuration(uint8_t duration, Channel& channel)
        {
            spdlog::debug("setupDuration({}, {})", duration, (long)(&channel - _channels));
            if (channel.durationRandomness) {
                channel.duration = duration + (getRandomNr() & channel.durationRandomness);
                return;
            }
            if (channel.fractionalSpacing)
                channel.spacing2 = (duration >> 3) * channel.fractionalSpacing;
            channel.duration = duration;
        }

        // This function may or may not play the note. It's usually followed by a call
        // to noteOn(), which will always play the current note.

        void ADLDriver::setupNote(uint8_t rawNote, Channel& channel, bool flag)
        {
            spdlog::debug("setupNote({}, {})", rawNote, (long)(&channel - _channels));

            if (_curChannel >= NUM_CHANNELS)
                return;

            channel.rawNote = rawNote;

            uint8_t note = (rawNote & 0x0F) + channel.baseNote;
            uint8_t octave = ((rawNote + channel.baseOctave) >> 4) & 0x0F;

            // There are only twelve notes. If we go outside that, we have to
            // adjust the note and octave.

            if (note >= 12) {
                octave += note / 12;
                note %= 12;
            }
            else if (note < 0) {
                uint8_t octaves = -(note + 1) / 12 + 1;
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

            if (channel.pitchBend || flag) {
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

            // Update octave & frequency, but keep on/off state.
            channel.regAx = freq & 0xFF;
            channel.regBx = (channel.regBx & 0x20) | (octave << 2) | ((freq >> 8) & 0x03);

            writeOPL(0xA0 + _curChannel, channel.regAx);
            writeOPL(0xB0 + _curChannel, channel.regBx);
        }

        void ADLDriver::setupInstrument(uint8_t regOffset, const uint8_t* dataptr, Channel& channel)
        {
            spdlog::debug("setupInstrument({}, {}, {})", regOffset, (const void*)dataptr, (long)(&channel - _channels));

            if (_curChannel >= NUM_CHANNELS)
                return;

            // Safety check: need 11 bytes of data.
            if (!checkDataOffset(dataptr, 11))
                return;

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

        // Apart from playing the note, this function also updates the variables for
        // the vibrato primary effect.

        void ADLDriver::noteOn(Channel& channel)
        {
            spdlog::debug("noteOn({})", (long)(&channel - _channels));

            // The "note on" bit is set, and the current note is played.

            if (_curChannel >= NUM_CHANNELS)
                return;

            channel.regBx |= 0x20;
            writeOPL(0xB0 + _curChannel, channel.regBx);

            // Update vibrato effect variables: vibratoStep is set to a
            // vibratoStepRange+1-bit value proportional to the note's f-number.
            // Reinitalize delay countdown; vibratoStepsCountdown reinitialization omitted.
            uint8_t shift = 9 - CLIP<uint8_t>(channel.vibratoStepRange, 0, 9);
            uint16_t freq = ((channel.regBx << 8) | channel.regAx) & 0x3FF;
            channel.vibratoStep = (freq >> shift) & 0xFF;
            channel.vibratoDelayCountdown = channel.vibratoDelay;
        }

        void ADLDriver::adjustVolume(Channel& channel)
        {
            spdlog::debug("adjustVolume({})", (long)(&channel - _channels));

            if (_curChannel >= NUM_CHANNELS)
                return;

            // Level Key Scaling / Total Level

            writeOPL(0x43 + _regOffset[_curChannel], calculateOpLevel2(channel));
            if (channel.twoChan)
                writeOPL(0x40 + _regOffset[_curChannel], calculateOpLevel1(channel));
        }

        uint8_t ADLDriver::calculateOpLevel1(Channel& channel)
        {
            int8_t value = channel.opLevel1 & 0x3F;

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
                spdlog::debug("ADLDriver::calculateOpLevel1(): WORKAROUND - total level clipping uint/int bug encountered");
            }

            value = CLIP<int8_t>(value, 0, 0x3F);

            if (!channel.volumeModifier)
                value = 0x3F;

            // Preserve the scaling level bits from opLevel1
            return value | (channel.opLevel1 & 0xC0);
        }

        uint8_t ADLDriver::calculateOpLevel2(Channel& channel) {
            int8_t value = channel.opLevel2 & 0x3F;

            value += channel.opExtraLevel1;
            value += channel.opExtraLevel2;

            uint16_t level3 = (channel.opExtraLevel3 ^ 0x3F) * channel.volumeModifier;
            if (level3) {
                level3 += 0x3F;
                level3 >>= 8;
            }

            value += level3 ^ 0x3F;

            // See comment in calculateOpLevel1()
            if (value & 0x80)
                spdlog::debug("ADLDriver::calculateOpLevel2(): WORKAROUND - total level clipping uint/int bug encountered");
            value = CLIP<int8_t>(value, 0, 0x3F);

            if (!channel.volumeModifier)
                value = 0x3F;

            // Preserve the scaling level bits from opLevel2
            return value | (channel.opLevel2 & 0xC0);
        }

        uint16_t ADLDriver::checkValue(int16_t val)
        {
            return CLIP<int16_t>(val, 0, 0x3F);
        }

        bool ADLDriver::advance(uint8_t& timer, uint8_t tempo)
        {
            uint8_t old = timer;
            timer += tempo;
            return timer < old;
        }

        const uint8_t* ADLDriver::checkDataOffset(const uint8_t* ptr, long n)
        {
            if (ptr)
            {
                long offset = ptr - _soundData.get();
                if (n >= -offset && n <= (long)_soundDataSize - offset)
                    return ptr + n;
            }

            return nullptr;
        }

        void ADLDriver::setupPrograms()
        {
            QueueEntry& entry = _programQueue[_programQueueStart];
            uint8_t* ptr = entry.data;

            // If there is no program queued, we skip this.
            if (_programQueueStart == _programQueueEnd && !ptr)
                return;

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
            if (!checkDataOffset(ptr, 2))
                return;

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

                if (chan <= 5)
                    channel.volumeModifier = _musicVolume;
                else
                    channel.volumeModifier = _sfxVolume;

                initAdlibChannel(chan);

                // We need to wait two callback calls till we can start another track.
                // This is (probably) required to assure that the sfx are started with
                // the correct priority and velocity.
                _programStartTimeout = 2;

                retrySound = QueueEntry();
            }

            if (retrySound.data)
            {
                spdlog::debug("ADLDriver::setupPrograms(): WORKAROUND - Restarting skipped sound {})", retrySound.id);
                startSound(retrySound.id, retrySound.volume);
            }
        }

        // A few words on opcode parsing and timing:
        //
        // First of all, we simulate a timer callback 72 times per second. Each timeout
        // we update each channel that has something to play.
        //
        // Each channel has its own individual tempo and timer. The timer is updated,
        // and when it wraps around, we go ahead and do more stuff with that channel.
        // Otherwise we skip straiht to the effect callbacks.
        //
        // Each channel also has a duration, indicating how much time is left on its
        // current task. This duration is decreased by one. As long as it still has
        // not reached zero, the only thing that can happen is that the note is turned
        // off depending on manual or automatic note spacing. Once the duration reaches
        // zero, a new set of musical opcodes are executed.
        //
        // An opcode is one byte, followed by a variable number of parameters.
        // If the most significant bit of the opcode is 1, it's a function; call it.
        // An opcode function can change control flow by updating the channel's data
        // pointer (which is set to the next opcode before the call). The function's
        // return value is either 0 (continue), 1 (stop) or 2 (stop, and do not run
        // the effects callbacks).
        //
        // If the most significant bit of the opcode is 0, it's a note, and the first
        // parameter is its duration. (There are cases where the duration is modified
        // but that's an exception.) The note opcode is assumed to return 1, and is the
        // last opcode unless its duration is zero.
        //
        // Finally, most of the times that the callback is called, it will invoke the
        // effects callbacks. The final opcode in a set can prevent this, if it's a
        // function and it returns anything other than 1.
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
                    for (_curChannel = NUM_CHANNELS; _curChannel >= 0; --_curChannel)
                    {
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
                    if (--channel.duration) {
                        if (channel.duration == channel.spacing2)
                            noteOff(channel);
                        if (channel.duration == channel.spacing1 && _curChannel != 9)
                            noteOff(channel);
                    }
                    else {
                        // Process some opcodes.
                        result = 0;
                    }
                }

                while (result == 0 && dataptr)
                {
                    int8_t opcode = 0xFF;
                    // Safety check to avoid illegal access.
                    // Stop channel if not enough data.
                    if (checkDataOffset(dataptr, 1)) {
                        opcode = *dataptr++;
                    }

                    if (opcode & 0x80)
                    {
                        opcode = CLIP<int8_t>(opcode & 0x7F, 0, _parserOpcodeTableSize - 1);
                        const ParserOpcode& op = _parserOpcodeTable[opcode];

                        // Safety check for end of data.
                        if (!checkDataOffset(dataptr, op.values))
                        {
                            result = update_stopChannel(channel, dataptr);
                            break;
                        }

                        spdlog::debug("Calling opcode '{}' ({}) (channel: {})", op.name, opcode, _curChannel);

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

                        int8_t duration = *dataptr++;
                        spdlog::debug("Note on opcode {:#04x} (duration: {}) (channel: {})", opcode, duration, _curChannel);

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

        // parser opcodes

        int ADLDriver::update_setRepeat(Channel& channel, const uint8_t* values) {
            channel.repeatCounter = values[0];
            return 0;
        }

        int ADLDriver::update_checkRepeat(Channel& channel, const uint8_t* values) {
            if (--channel.repeatCounter) {
                int16_t add = READ_LE_UINT16(values);

                // Safety check: ignore jump to invalid address
                if (!checkDataOffset(channel.dataptr, add))
                    spdlog::warn("ADLDriver::update_checkRepeat: Ignoring invalid offset {}", add);
                else
                    channel.dataptr += add;
            }
            return 0;
        }

        int ADLDriver::update_setupProgram(Channel& channel, const uint8_t* values) {
            if (values[0] == 0xFF)
                return 0;

            const uint8_t* ptr = getProgram(values[0]);

            // In case we encounter an invalid program we simply ignore it and do
            // nothing instead. The original did not care about invalid programs and
            // simply tried to play them anyway... But to avoid crashes due we ingore
            // them.
            // This, for example, happens in the Lands of Lore intro when Scotia gets
            // the ring in the intro.
            if (!checkDataOffset(ptr, 2)) {
                spdlog::debug("ADLDriver::update_setupProgram: Invalid program {} specified", values[0]);
                return 0;
            }

            int8_t chan = *ptr++;
            int8_t priority = *ptr++;

            // Safety check: ignore programs with invalid channel number.
            if (chan > 9) {
                spdlog::warn("ADLDriver::update_setupProgram: Invalid channel {}", chan);
                return 0;
            }

            Channel& channel2 = _channels[chan];

            if (priority >= channel2.priority) {
                // The opcode is not allowed to modify its own data pointer.
                // To enforce that, we make a copy and restore it later.
                //
                // This fixes a subtle music bug where the wrong music would
                // play when getting the quill in Kyra 1.
                const uint8_t* dataptrBackUp = channel.dataptr;

                // We keep new tracks from being started for two further iterations of
                // the callback. This assures the correct velocity is used for this
                // program.
                _programStartTimeout = 2;

                initChannel(channel2);
                channel2.priority = priority;
                channel2.dataptr = ptr;
                channel2.tempo = 0xFF;
                channel2.timer = 0xFF;
                channel2.duration = 1;

                if (chan <= 5)
                    channel2.volumeModifier = _musicVolume;
                else
                    channel2.volumeModifier = _sfxVolume;

                initAdlibChannel(chan);

                channel.dataptr = dataptrBackUp;
            }

            return 0;
        }

        int ADLDriver::update_setNoteSpacing(Channel& channel, const uint8_t* values) {
            channel.spacing1 = values[0];
            return 0;
        }

        int ADLDriver::update_jump(Channel& channel, const uint8_t* values) {
            int16_t add = READ_LE_UINT16(values);
            // Safety check: ignore jump to invalid address
            if (_version == 1)
                channel.dataptr = checkDataOffset(_soundData.get(), add - 191);
            else
                channel.dataptr = checkDataOffset(channel.dataptr, add);

            if (!channel.dataptr) {
                spdlog::warn("ADLDriver::update_jump: Invalid offset {}, stopping channel", add);
                return update_stopChannel(channel, values);
            }
            if (_syncJumpMask & (1 << (&channel - _channels)))
                channel.lock = true;
            return 0;
        }

        int ADLDriver::update_jumpToSubroutine(Channel& channel, const uint8_t* values) {
            int16_t add = READ_LE_UINT16(values);

            // Safety checks: ignore jumps when stack is full or address is invalid.
            if (channel.dataptrStackPos >= ARRAYSIZE(channel.dataptrStack)) {
                spdlog::warn("ADLDriver::update_jumpToSubroutine: Stack overlow");
                return 0;
            }
            channel.dataptrStack[channel.dataptrStackPos++] = channel.dataptr;
            if (_version < 3)
                channel.dataptr = checkDataOffset(_soundData.get(), add - 191);
            else
                channel.dataptr = checkDataOffset(channel.dataptr, add);

            if (!channel.dataptr)
                channel.dataptr = channel.dataptrStack[--channel.dataptrStackPos];
            return 0;
        }

        int ADLDriver::update_returnFromSubroutine(Channel& channel, const uint8_t* values) {
            // Safety check: stop track when stack is empty.
            if (!channel.dataptrStackPos) {
                spdlog::warn("ADLDriver::update_returnFromSubroutine: Stack underflow");
                return update_stopChannel(channel, values);
            }
            channel.dataptr = channel.dataptrStack[--channel.dataptrStackPos];
            return 0;
        }

        int ADLDriver::update_setBaseOctave(Channel& channel, const uint8_t* values) {
            channel.baseOctave = values[0];
            return 0;
        }

        int ADLDriver::update_stopChannel(Channel& channel, const uint8_t* values) {
            channel.priority = 0;
            if (_curChannel != 9)
                noteOff(channel);
            channel.dataptr = nullptr;
            return 2;
        }

        int ADLDriver::update_playRest(Channel& channel, const uint8_t* values) {
            setupDuration(values[0], channel);
            noteOff(channel);
            return values[0] != 0;
        }

        int ADLDriver::update_writeAdLib(Channel& channel, const uint8_t* values) {
            writeOPL(values[0], values[1]);
            return 0;
        }

        int ADLDriver::update_setupNoteAndDuration(Channel& channel, const uint8_t* values) {
            setupNote(values[0], channel);
            setupDuration(values[1], channel);
            return values[1] != 0;
        }

        int ADLDriver::update_setBaseNote(Channel& channel, const uint8_t* values) {
            channel.baseNote = values[0];
            return 0;
        }

        int ADLDriver::update_setupSecondaryEffect1(Channel& channel, const uint8_t* values) {
            channel.secondaryEffectTimer = channel.secondaryEffectTempo = values[0];
            channel.secondaryEffectSize = channel.secondaryEffectPos = values[1];
            channel.secondaryEffectRegbase = values[2];
            // WORKAROUND: The original code reads a true offset which later gets translated via xlat (in
            // the current segment). This means that the outcome depends on the sound data offset.
            // Unfortunately this offset is different in most implementations of the audio driver and
            // probably also different from the offset assumed by the sequencer.
            // It seems that the driver assumes an offset of 191 which is wrong for all the game driver
            // implementations.
            // This bug has probably not been noticed, since the effect is hardly used and the sounds are
            // not necessarily worse. I noticed the difference between ScummVM and DOSBox for the EOB II
            // teleporter sound. I also found the location of the table which is supposed to be used here
            // (simple enough: it is located at the end of the track after the 0x88 ending opcode).
            // Teleporters in EOB I and II now sound exactly the same which I am sure was the intended way,
            // since the sound data is exactly the same.
            // In DOSBox the teleporters will sound different in EOB I and II, due to different sound
            // data offsets.
            channel.secondaryEffectData = READ_LE_UINT16(&values[3]) - 191;
            channel.secondaryEffect = &ADLDriver::secondaryEffect1;

            // Safety check: don't enable effect when table location is invalid.
            int start = channel.secondaryEffectData + channel.secondaryEffectSize;
            if (start < 0 || start >= (int)_soundDataSize) {
                spdlog::warn("ADLDriver::update_setupSecondaryEffect1: Ignoring due to invalid table location");
                channel.secondaryEffect = nullptr;
            }
            return 0;
        }

        int ADLDriver::update_stopOtherChannel(Channel& channel, const uint8_t* values)
        {
            // Safety check
            if (values[0] > NUM_CHANNELS) {
                spdlog::warn("ADLDriver::update_stopOtherChannel: Ignoring invalid channel {}", values[0]);
                return 0;
            }

            // Don't change our own dataptr!
            const uint8_t* dataptrBackUp = channel.dataptr;

            Channel& channel2 = _channels[values[0]];
            channel2.duration = 0;
            channel2.priority = 0;
            channel2.dataptr = nullptr;

            channel.dataptr = dataptrBackUp;
            return 0;
        }

        int ADLDriver::update_waitForEndOfProgram(Channel& channel, const uint8_t* values)
        {
            const uint8_t* ptr = getProgram(values[0]);

            // Safety check in case an invalid program is specified. This would make
            // getProgram return a nullptr and thus cause invalid memory reads.
            if (!ptr) {
                spdlog::debug("ADLDriver::update_waitForEndOfProgram: Invalid program {} specified", values[0]);
                return 0;
            }

            int8_t chan = *ptr;

            if (chan > NUM_CHANNELS || !_channels[chan].dataptr)
                return 0;

            channel.dataptr -= 2;
            return 2;
        }

        int ADLDriver::update_setupInstrument(Channel& channel, const uint8_t* values) {
            const uint8_t* instrument = getInstrument(values[0]);

            // We add a safety check to avoid setting up invalid instruments. This is
            // not done in the original. However, to avoid crashes due to invalid
            // memory reads we simply ignore the request.
            // This happens, for example, in Hand of Fate when using the swampsnake
            // potion on Zanthia to scare off the rat in the cave in the first chapter
            // of the game.
            if (!instrument) {
                spdlog::debug("ADLDriver::update_setupInstrument: Invalid instrument {} specified", values[0]);
                return 0;
            }

            setupInstrument(_curRegOffset, instrument, channel);
            return 0;
        }

        int ADLDriver::update_setupPrimaryEffectSlide(Channel& channel, const uint8_t* values) {
            channel.slideTempo = values[0];
            channel.slideStep = READ_BE_UINT16(&values[1]);
            channel.primaryEffect = &ADLDriver::primaryEffectSlide;
            channel.slideTimer = 0xFF;
            return 0;
        }

        int ADLDriver::update_removePrimaryEffectSlide(Channel& channel, const uint8_t* values) {
            channel.primaryEffect = nullptr;
            channel.slideStep = 0;
            return 0;
        }

        int ADLDriver::update_setBaseFreq(Channel& channel, const uint8_t* values) {
            channel.baseFreq = values[0];
            return 0;
        }

        int ADLDriver::update_setupPrimaryEffectVibrato(Channel& channel, const uint8_t* values) {
            channel.vibratoTempo = values[0];
            channel.vibratoStepRange = values[1];
            channel.vibratoStepsCountdown = values[2] + 1;
            channel.vibratoNumSteps = values[2] << 1;
            channel.vibratoDelay = values[3];
            channel.primaryEffect = &ADLDriver::primaryEffectVibrato;
            return 0;
        }

        int ADLDriver::update_setPriority(Channel& channel, const uint8_t* values) {
            channel.priority = values[0];
            return 0;
        }

        // This provides a way to synchronize channels with a global beat:
        //
        // update_setBeat()
        //    - Initializes _beatDivider, _beatDivCnt, _beatCounter, and _beatWaiting;
        //      resets _callbackTimer
        //    - _beatDivider is not further modified
        //
        // callback()
        //    - _beatDivCnt is a countdown, gets reinitialized to _beatDivider on zero
        //    - _beatCounter is incremented when _beatDivCnt is reset, i.e., it's a
        //      counter which updates with the global _tempo divided by _beatDivider.
        //
        // update_waitForNextBeat()
        //    - _beatWaiting is updated if some bits are 0 in _beatCounter (off beat)
        //    - the program is stopped until some of the masked bits in _beatCounter
        //      become 1 and _beatWaiting is non-zero (on beat), then _beatWaiting is
        //      cleared
        //
        // _beatDivider - determines how fast _beatCounter is incremented
        // _beatDivCnt - countdown for the divider
        // _beatCounter - counter updated with global _tempo divided by _beatDivider
        // _beatWaiting - flags that waiting started before watched counter bit got 1
        //
        // Note that in theory _beatWaiting could wrap around to zero while waiting,
        // then the rising edge wouldn't trigger. That's probably not a big issue
        // in practice sice it can only happen for long delays (big _beatDivider and
        // waiting on one of the higher bits) but could have been prevented easily.
        int ADLDriver::update_setBeat(Channel& channel, const uint8_t* values) {
            _beatDivider = _beatDivCnt = values[0] >> 1;
            _callbackTimer = 0xFF;
            _beatCounter = _beatWaiting = 0;
            return 0;
        }

        int ADLDriver::update_waitForNextBeat(Channel& channel, const uint8_t* values) {
            if ((_beatCounter & values[0]) && _beatWaiting) {
                _beatWaiting = 0;
                return 0;
            }

            if (!(_beatCounter & values[0]))
                ++_beatWaiting;

            channel.dataptr -= 2;
            channel.duration = 1;
            return 2;
        }

        int ADLDriver::update_setExtraLevel1(Channel& channel, const uint8_t* values) {
            channel.opExtraLevel1 = values[0];
            adjustVolume(channel);
            return 0;
        }

        int ADLDriver::update_setupDuration(Channel& channel, const uint8_t* values) {
            setupDuration(values[0], channel);
            return values[0] != 0;
        }

        int ADLDriver::update_playNote(Channel& channel, const uint8_t* values) {
            setupDuration(values[0], channel);
            noteOn(channel);
            return values[0] != 0;
        }

        int ADLDriver::update_setFractionalNoteSpacing(Channel& channel, const uint8_t* values) {
            channel.fractionalSpacing = values[0] & 7;
            return 0;
        }

        int ADLDriver::update_setTempo(Channel& channel, const uint8_t* values) {
            _tempo = values[0];
            return 0;
        }

        int ADLDriver::update_removeSecondaryEffect1(Channel& channel, const uint8_t* values) {
            channel.secondaryEffect = nullptr;
            return 0;
        }

        int ADLDriver::update_setChannelTempo(Channel& channel, const uint8_t* values) {
            channel.tempo = values[0];
            return 0;
        }

        int ADLDriver::update_setExtraLevel3(Channel& channel, const uint8_t* values) {
            channel.opExtraLevel3 = values[0];
            return 0;
        }

        int ADLDriver::update_setExtraLevel2(Channel& channel, const uint8_t* values)
        {
            // Safety check
            if (values[0] > NUM_CHANNELS) {
                spdlog::warn("ADLDriver::update_setExtraLevel2: Ignore invalid channel {}", values[0]);
                return 0;
            }

            int channelBackUp = _curChannel;

            _curChannel = values[0];
            Channel& channel2 = _channels[_curChannel];
            channel2.opExtraLevel2 = values[1];
            adjustVolume(channel2);

            _curChannel = channelBackUp;
            return 0;
        }

        int ADLDriver::update_changeExtraLevel2(Channel& channel, const uint8_t* values)
        {
            // Safety check
            if (values[0] > NUM_CHANNELS) {
                spdlog::warn("ADLDriver::update_changeExtraLevel2: Ignore invalid channel {}", values[0]);
                return 0;
            }

            int channelBackUp = _curChannel;

            _curChannel = values[0];
            Channel& channel2 = _channels[_curChannel];
            channel2.opExtraLevel2 += values[1];
            adjustVolume(channel2);

            _curChannel = channelBackUp;
            return 0;
        }

        // Apart from initializing to zero, these two functions are the only ones that
        // modify _vibratoAndAMDepthBits.
        int ADLDriver::update_setAMDepth(Channel& channel, const uint8_t* values) {
            if (values[0] & 1)
                _vibratoAndAMDepthBits |= 0x80;
            else
                _vibratoAndAMDepthBits &= 0x7F;

            writeOPL(0xBD, _vibratoAndAMDepthBits);
            return 0;
        }

        int ADLDriver::update_setVibratoDepth(Channel& channel, const uint8_t* values) {
            if (values[0] & 1)
                _vibratoAndAMDepthBits |= 0x40;
            else
                _vibratoAndAMDepthBits &= 0xBF;

            writeOPL(0xBD, _vibratoAndAMDepthBits);
            return 0;
        }

        int ADLDriver::update_changeExtraLevel1(Channel& channel, const uint8_t* values) {
            channel.opExtraLevel1 += values[0];
            adjustVolume(channel);
            return 0;
        }

        int ADLDriver::update_clearChannel(Channel& channel, const uint8_t* values)
        {
            // Safety check
            if (values[0] > NUM_CHANNELS) {
                spdlog::warn("ADLDriver::update_clearChannel: Ignore invalid channel {}", values[0]);
                return 0;
            }

            int channelBackUp = _curChannel;
            _curChannel = values[0];
            // Don't modify our own dataptr!
            const uint8_t* dataptrBackUp = channel.dataptr;

            // Stop channel
            Channel& channel2 = _channels[_curChannel];
            channel2.duration = channel2.priority = 0;
            channel2.dataptr = 0;
            channel2.opExtraLevel2 = 0;

            if (_curChannel != NUM_CHANNELS)
            {
                // Silence channel
                int8_t regOff = _regOffset[_curChannel];

                // Feedback strength / Connection type
                writeOPL(0xC0 + _curChannel, 0x00);

                // Key scaling level / Operator output level
                writeOPL(0x43 + regOff, 0x3F);

                // Sustain Level / Release Rate
                writeOPL(0x83 + regOff, 0xFF);

                // Key On / Octave / Frequency
                writeOPL(0xB0 + _curChannel, 0x00);
            }

            _curChannel = channelBackUp;
            channel.dataptr = dataptrBackUp;
            return 0;
        }

        int ADLDriver::update_changeNoteRandomly(Channel& channel, const uint8_t* values)
        {
            if (_curChannel >= NUM_CHANNELS)
                return 0;

            uint16_t mask = READ_BE_UINT16(values);

            uint16_t note = ((channel.regBx & 0x1F) << 8) | channel.regAx;

            note += mask & getRandomNr();
            note |= ((channel.regBx & 0x20) << 8);

            // Frequency
            writeOPL(0xA0 + _curChannel, note & 0xFF);

            // Key On / Octave / Frequency
            writeOPL(0xB0 + _curChannel, (note & 0xFF00) >> 8);

            return 0;
        }

        int ADLDriver::update_removePrimaryEffectVibrato(Channel& channel, const uint8_t* values) {
            channel.primaryEffect = nullptr;
            return 0;
        }

        int ADLDriver::update_pitchBend(Channel& channel, const uint8_t* values) {
            channel.pitchBend = (int8_t)values[0];
            setupNote(channel.rawNote, channel, true);
            return 0;
        }

        int ADLDriver::update_resetToGlobalTempo(Channel& channel, const uint8_t* values) {
            channel.tempo = _tempo;
            return 0;
        }

        int ADLDriver::update_nop(Channel& channel, const uint8_t* values) {
            return 0;
        }

        int ADLDriver::update_setDurationRandomness(Channel& channel, const uint8_t* values) {
            channel.durationRandomness = values[0];
            return 0;
        }

        int ADLDriver::update_changeChannelTempo(Channel& channel, const uint8_t* values) {
            channel.tempo = CLIP(channel.tempo + (int8_t)values[0], 1, 255);
            return 0;
        }

        int ADLDriver::updateCallback46(Channel& channel, const uint8_t* values) {
            int8_t entry = values[1];

            // Safety check: prevent illegal table access
            if (entry + 2 > _unkTable2Size)
                return 0;

            _tablePtr1 = _unkTable2[entry];
            _tablePtr2 = _unkTable2[entry + 1];
            if (values[0] == 2) {
                // Frequency
                writeOPL(0xA0, _tablePtr2[0]);
            }
            return 0;
        }

        int ADLDriver::update_setupRhythmSection(Channel& channel, const uint8_t* values) {
            int channelBackUp = _curChannel;
            int regOffsetBackUp = _curRegOffset;

            _curChannel = 6;
            _curRegOffset = _regOffset[6];

            const uint8_t* instrument;
            instrument = getInstrument(values[0]);
            if (instrument) {
                setupInstrument(_curRegOffset, instrument, channel);
            }
            else {
                spdlog::debug("ADLDriver::update_setupRhythmSection: Invalid instrument {} for channel 6 specified", values[0]);
            }
            _opLevelBD = channel.opLevel2;

            _curChannel = 7;
            _curRegOffset = _regOffset[7];

            instrument = getInstrument(values[1]);
            if (instrument) {
                setupInstrument(_curRegOffset, instrument, channel);
            }
            else {
                spdlog::debug("ADLDriver::update_setupRhythmSection: Invalid instrument {} for channel 7 specified", values[1]);
            }
            _opLevelHH = channel.opLevel1;
            _opLevelSD = channel.opLevel2;

            _curChannel = 8;
            _curRegOffset = _regOffset[8];

            instrument = getInstrument(values[2]);
            if (instrument) {
                setupInstrument(_curRegOffset, instrument, channel);
            }
            else {
                spdlog::debug("ADLDriver::update_setupRhythmSection: Invalid instrument {} for channel 8 specified", values[2]);
            }
            _opLevelTT = channel.opLevel1;
            _opLevelCY = channel.opLevel2;

            // Octave / F-Number / Key-On for channels 6, 7 and 8

            _channels[6].regBx = values[3] & 0x2F;
            writeOPL(0xB6, _channels[6].regBx);
            writeOPL(0xA6, values[4]);

            _channels[7].regBx = values[5] & 0x2F;
            writeOPL(0xB7, _channels[7].regBx);
            writeOPL(0xA7, values[6]);

            _channels[8].regBx = values[7] & 0x2F;
            writeOPL(0xB8, _channels[8].regBx);
            writeOPL(0xA8, values[8]);

            _rhythmSectionBits = 0x20;

            _curRegOffset = regOffsetBackUp;
            _curChannel = channelBackUp;
            return 0;
        }

        int ADLDriver::update_playRhythmSection(Channel& channel, const uint8_t* values) {
            // Any instrument that we want to play, and which was already playing,
            // is temporarily keyed off. Instruments that were off already, or
            // which we don't want to play, retain their old on/off status. This is
            // probably so that the instrument's envelope is played from its
            // beginning again...

            writeOPL(0xBD, (_rhythmSectionBits & ~(values[0] & 0x1F)) | 0x20);

            // ...but since we only set the rhythm instrument bits, and never clear
            // them (until the entire rhythm section is disabled), I'm not sure how
            // useful the cleverness above is. We could perhaps simply turn off all
            // the rhythm instruments instead.

            _rhythmSectionBits |= values[0];

            writeOPL(0xBD, _vibratoAndAMDepthBits | 0x20 | _rhythmSectionBits);
            return 0;
        }

        int ADLDriver::update_removeRhythmSection(Channel& channel, const uint8_t* values) {
            _rhythmSectionBits = 0;

            // All the rhythm bits are cleared. The AM and Vibrato depth bits
            // remain unchanged.

            writeOPL(0xBD, _vibratoAndAMDepthBits);
            return 0;
        }

        int ADLDriver::update_setRhythmLevel2(Channel& channel, const uint8_t* values) {
            int8_t ops = values[0], v = values[1];

            if (ops & 1) {
                _opExtraLevel2HH = v;

                // Channel 7, op1: Level Key Scaling / Total Level
                writeOPL(0x51, checkValue(v + _opLevelHH + _opExtraLevel1HH + _opExtraLevel2HH));
            }

            if (ops & 2) {
                _opExtraLevel2CY = v;

                // Channel 8, op2: Level Key Scaling / Total Level
                writeOPL(0x55, checkValue(v + _opLevelCY + _opExtraLevel1CY + _opExtraLevel2CY));
            }

            if (ops & 4) {
                _opExtraLevel2TT = v;

                // Channel 8, op1: Level Key Scaling / Total Level
                writeOPL(0x52, checkValue(v + _opLevelTT + _opExtraLevel1TT + _opExtraLevel2TT));
            }

            if (ops & 8) {
                _opExtraLevel2SD = v;

                // Channel 7, op2: Level Key Scaling / Total Level
                writeOPL(0x54, checkValue(v + _opLevelSD + _opExtraLevel1SD + _opExtraLevel2SD));
            }

            if (ops & 16) {
                _opExtraLevel2BD = v;

                // Channel 6, op2: Level Key Scaling / Total Level
                writeOPL(0x53, checkValue(v + _opLevelBD + _opExtraLevel1BD + _opExtraLevel2BD));
            }

            return 0;
        }

        int ADLDriver::update_changeRhythmLevel1(Channel& channel, const uint8_t* values) {
            int8_t ops = values[0], v = values[1];

            if (ops & 1) {
                _opExtraLevel1HH = checkValue(v + _opLevelHH + _opExtraLevel1HH + _opExtraLevel2HH);

                // Channel 7, op1: Level Key Scaling / Total Level
                writeOPL(0x51, _opExtraLevel1HH);
            }

            if (ops & 2) {
                _opExtraLevel1CY = checkValue(v + _opLevelCY + _opExtraLevel1CY + _opExtraLevel2CY);

                // Channel 8, op2: Level Key Scaling / Total Level
                writeOPL(0x55, _opExtraLevel1CY);
            }

            if (ops & 4) {
                _opExtraLevel1TT = checkValue(v + _opLevelTT + _opExtraLevel1TT + _opExtraLevel2TT);

                // Channel 8, op1: Level Key Scaling / Total Level
                writeOPL(0x52, _opExtraLevel1TT);
            }

            if (ops & 8) {
                _opExtraLevel1SD = checkValue(v + _opLevelSD + _opExtraLevel1SD + _opExtraLevel2SD);

                // Channel 7, op2: Level Key Scaling / Total Level
                writeOPL(0x54, _opExtraLevel1SD);
            }

            if (ops & 16) {
                _opExtraLevel1BD = checkValue(v + _opLevelBD + _opExtraLevel1BD + _opExtraLevel2BD);

                // Channel 6, op2: Level Key Scaling / Total Level
                writeOPL(0x53, _opExtraLevel1BD);
            }

            return 0;
        }

        int ADLDriver::update_setRhythmLevel1(Channel& channel, const uint8_t* values) {
            int8_t ops = values[0], v = values[1];

            if (ops & 1) {
                _opExtraLevel1HH = v;

                // Channel 7, op1: Level Key Scaling / Total Level
                writeOPL(0x51, checkValue(v + _opLevelHH + _opExtraLevel2HH));
            }

            if (ops & 2) {
                _opExtraLevel1CY = v;

                // Channel 8, op2: Level Key Scaling / Total Level
                writeOPL(0x55, checkValue(v + _opLevelCY + _opExtraLevel2CY));
            }

            if (ops & 4) {
                _opExtraLevel1TT = v;

                // Channel 8, op1: Level Key Scaling / Total Level
                writeOPL(0x52, checkValue(v + _opLevelTT + _opExtraLevel2TT));
            }

            if (ops & 8) {
                _opExtraLevel1SD = v;

                // Channel 7, op2: Level Key Scaling / Total Level
                writeOPL(0x54, checkValue(v + _opLevelSD + _opExtraLevel2SD));
            }

            if (ops & 16) {
                _opExtraLevel1BD = v;

                // Channel 6, op2: Level Key Scaling / Total Level
                writeOPL(0x53, checkValue(v + _opLevelBD + _opExtraLevel2BD));
            }

            return 0;
        }

        int ADLDriver::update_setSoundTrigger(Channel& channel, const uint8_t* values) {
            _soundTrigger = values[0];
            return 0;
        }

        int ADLDriver::update_setTempoReset(Channel& channel, const uint8_t* values) {
            channel.tempoReset = values[0];
            return 0;
        }

        int ADLDriver::updateCallback56(Channel& channel, const uint8_t* values) {
            channel.unk39 = values[0];
            channel.unk40 = values[1];
            return 0;
        }

        // static res

#define COMMAND(x, n) { &ADLDriver::x, #x, n }

        const ADLDriver::ParserOpcode ADLDriver::_parserOpcodeTable[] = {
            // 0
            COMMAND(update_setRepeat, 1),
            COMMAND(update_checkRepeat, 2),
            COMMAND(update_setupProgram, 1),
            COMMAND(update_setNoteSpacing, 1),

            // 4
            COMMAND(update_jump, 2),
            COMMAND(update_jumpToSubroutine, 2),
            COMMAND(update_returnFromSubroutine, 0),
            COMMAND(update_setBaseOctave, 1),

            // 8
            COMMAND(update_stopChannel, 0),
            COMMAND(update_playRest, 1),
            COMMAND(update_writeAdLib, 2),
            COMMAND(update_setupNoteAndDuration, 2),

            // 12
            COMMAND(update_setBaseNote, 1),
            COMMAND(update_setupSecondaryEffect1, 5),
            COMMAND(update_stopOtherChannel, 1),
            COMMAND(update_waitForEndOfProgram, 1),

            // 16
            COMMAND(update_setupInstrument, 1),
            COMMAND(update_setupPrimaryEffectSlide, 3),
            COMMAND(update_removePrimaryEffectSlide, 0),
            COMMAND(update_setBaseFreq, 1),

            // 20
            COMMAND(update_stopChannel, 0),
            COMMAND(update_setupPrimaryEffectVibrato, 4),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_stopChannel, 0),

            // 24
            COMMAND(update_stopChannel, 0),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_setPriority, 1),
            COMMAND(update_stopChannel, 0),

            // 28
            COMMAND(update_setBeat, 1),
            COMMAND(update_waitForNextBeat, 1),
            COMMAND(update_setExtraLevel1, 1),
            COMMAND(update_stopChannel, 0),

            // 32
            COMMAND(update_setupDuration, 1),
            COMMAND(update_playNote, 1),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_stopChannel, 0),

            // 36
            COMMAND(update_setFractionalNoteSpacing, 1),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_setTempo, 1),
            COMMAND(update_removeSecondaryEffect1, 0),

            // 40
            COMMAND(update_stopChannel, 0),
            COMMAND(update_setChannelTempo, 1),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_setExtraLevel3, 1),

            // 44
            COMMAND(update_setExtraLevel2, 2),
            COMMAND(update_changeExtraLevel2, 2),
            COMMAND(update_setAMDepth, 1),
            COMMAND(update_setVibratoDepth, 1),

            // 48
            COMMAND(update_changeExtraLevel1, 1),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_stopChannel, 0),
            COMMAND(update_clearChannel, 1),

            // 52
            COMMAND(update_stopChannel, 0),
            COMMAND(update_changeNoteRandomly, 2),
            COMMAND(update_removePrimaryEffectVibrato, 0),
            COMMAND(update_stopChannel, 0),

            // 56
            COMMAND(update_stopChannel, 0),
            COMMAND(update_pitchBend, 1),
            COMMAND(update_resetToGlobalTempo, 0),
            COMMAND(update_nop, 0),

            // 60
            COMMAND(update_setDurationRandomness, 1),
            COMMAND(update_changeChannelTempo, 1),
            COMMAND(update_stopChannel, 0),
            COMMAND(updateCallback46, 2),

            // 64
            COMMAND(update_nop, 0),
            COMMAND(update_setupRhythmSection, 9),
            COMMAND(update_playRhythmSection, 1),
            COMMAND(update_removeRhythmSection, 0),

            // 68
            COMMAND(update_setRhythmLevel2, 2),
            COMMAND(update_changeRhythmLevel1, 2),
            COMMAND(update_setRhythmLevel1, 2),
            COMMAND(update_setSoundTrigger, 1),

            // 72
            COMMAND(update_setTempoReset, 1),
            COMMAND(updateCallback56, 2),
            COMMAND(update_stopChannel, 0)
        };

#undef COMMAND

        const int ADLDriver::_parserOpcodeTableSize = ARRAYSIZE(ADLDriver::_parserOpcodeTable);

        // This table holds the register offset for operator 1 for each of the nine
        // channels. To get the register offset for operator 2, simply add 3.

        const uint8_t ADLDriver::_regOffset[] = {
            0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11,
            0x12
        };

        // These are the F-Numbers (10 bits) for the notes of the 12-tone scale.
        // However, it does not match the table in the AdLib documentation I've seen.

        const uint16_t ADLDriver::_freqTable[] = {
            0x0134, 0x0147, 0x015A, 0x016F, 0x0184, 0x019C, 0x01B4, 0x01CE, 0x01E9,
            0x0207, 0x0225, 0x0246
        };

        // These tables are currently only used by updateCallback46(), which only ever
        // uses the first element of one of the sub-tables.

        const uint8_t* const ADLDriver::_unkTable2[] = {
            ADLDriver::_unkTable2_1,
            ADLDriver::_unkTable2_2,
            ADLDriver::_unkTable2_1,
            ADLDriver::_unkTable2_2,
            ADLDriver::_unkTable2_3,
            ADLDriver::_unkTable2_2
        };

        const int ADLDriver::_unkTable2Size = ARRAYSIZE(ADLDriver::_unkTable2);

        const uint8_t ADLDriver::_unkTable2_1[] = {
            0x50, 0x50, 0x4F, 0x4F, 0x4E, 0x4E, 0x4D, 0x4D,
            0x4C, 0x4C, 0x4B, 0x4B, 0x4A, 0x4A, 0x49, 0x49,
            0x48, 0x48, 0x47, 0x47, 0x46, 0x46, 0x45, 0x45,
            0x44, 0x44, 0x43, 0x43, 0x42, 0x42, 0x41, 0x41,
            0x40, 0x40, 0x3F, 0x3F, 0x3E, 0x3E, 0x3D, 0x3D,
            0x3C, 0x3C, 0x3B, 0x3B, 0x3A, 0x3A, 0x39, 0x39,
            0x38, 0x38, 0x37, 0x37, 0x36, 0x36, 0x35, 0x35,
            0x34, 0x34, 0x33, 0x33, 0x32, 0x32, 0x31, 0x31,
            0x30, 0x30, 0x2F, 0x2F, 0x2E, 0x2E, 0x2D, 0x2D,
            0x2C, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A, 0x29, 0x29,
            0x28, 0x28, 0x27, 0x27, 0x26, 0x26, 0x25, 0x25,
            0x24, 0x24, 0x23, 0x23, 0x22, 0x22, 0x21, 0x21,
            0x20, 0x20, 0x1F, 0x1F, 0x1E, 0x1E, 0x1D, 0x1D,
            0x1C, 0x1C, 0x1B, 0x1B, 0x1A, 0x1A, 0x19, 0x19,
            0x18, 0x18, 0x17, 0x17, 0x16, 0x16, 0x15, 0x15,
            0x14, 0x14, 0x13, 0x13, 0x12, 0x12, 0x11, 0x11,
            0x10, 0x10
        };

        // no don't ask me WHY this table exsits!
        const uint8_t ADLDriver::_unkTable2_2[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
            0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
            0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x6F,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
            0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
            0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
        };

        const uint8_t ADLDriver::_unkTable2_3[] = {
            0x40, 0x40, 0x40, 0x3F, 0x3F, 0x3F, 0x3E, 0x3E,
            0x3E, 0x3D, 0x3D, 0x3D, 0x3C, 0x3C, 0x3C, 0x3B,
            0x3B, 0x3B, 0x3A, 0x3A, 0x3A, 0x39, 0x39, 0x39,
            0x38, 0x38, 0x38, 0x37, 0x37, 0x37, 0x36, 0x36,
            0x36, 0x35, 0x35, 0x35, 0x34, 0x34, 0x34, 0x33,
            0x33, 0x33, 0x32, 0x32, 0x32, 0x31, 0x31, 0x31,
            0x30, 0x30, 0x30, 0x2F, 0x2F, 0x2F, 0x2E, 0x2E,
            0x2E, 0x2D, 0x2D, 0x2D, 0x2C, 0x2C, 0x2C, 0x2B,
            0x2B, 0x2B, 0x2A, 0x2A, 0x2A, 0x29, 0x29, 0x29,
            0x28, 0x28, 0x28, 0x27, 0x27, 0x27, 0x26, 0x26,
            0x26, 0x25, 0x25, 0x25, 0x24, 0x24, 0x24, 0x23,
            0x23, 0x23, 0x22, 0x22, 0x22, 0x21, 0x21, 0x21,
            0x20, 0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1E, 0x1E,
            0x1E, 0x1D, 0x1D, 0x1D, 0x1C, 0x1C, 0x1C, 0x1B,
            0x1B, 0x1B, 0x1A, 0x1A, 0x1A, 0x19, 0x19, 0x19,
            0x18, 0x18, 0x18, 0x17, 0x17, 0x17, 0x16, 0x16,
            0x16, 0x15
        };

        // This table is used to modify the frequency of the notes, depending on the
        // note value and the pitch bend value. In theory, we could very well try to
        // access memory outside this table, but in reality that probably won't happen.
        //
        const uint8_t ADLDriver::_pitchBendTables[][32] = {
            // 0
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08,
              0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
              0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x19,
              0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21 },
            // 1
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x09,
              0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
              0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x1A,
              0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x24 },
            // 2
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x08, 0x09,
              0x0A, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x12, 0x13,
              0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1C, 0x1D,
              0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x25, 0x26 },
            // 3
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x08, 0x0A,
              0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x12, 0x13,
              0x14, 0x15, 0x16, 0x17, 0x18, 0x1A, 0x1C, 0x1D,
              0x1E, 0x1F, 0x20, 0x21, 0x23, 0x25, 0x27, 0x28 },
            // 4
            { 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x08, 0x0A,
              0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x13, 0x15,
              0x16, 0x17, 0x18, 0x19, 0x1B, 0x1D, 0x1F, 0x20,
              0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x28, 0x2A },
            // 5
            { 0x00, 0x01, 0x02, 0x03, 0x05, 0x07, 0x09, 0x0B,
              0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x13, 0x15,
              0x16, 0x17, 0x18, 0x19, 0x1B, 0x1D, 0x1F, 0x20,
              0x21, 0x22, 0x23, 0x25, 0x27, 0x29, 0x2B, 0x2D },
            // 6
            { 0x00, 0x01, 0x02, 0x03, 0x05, 0x07, 0x09, 0x0B,
              0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x13, 0x15,
              0x16, 0x17, 0x18, 0x1A, 0x1C, 0x1E, 0x21, 0x24,
              0x25, 0x26, 0x27, 0x29, 0x2B, 0x2D, 0x2F, 0x30 },
            // 7
            { 0x00, 0x01, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C,
              0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x13, 0x15, 0x18,
              0x19, 0x1A, 0x1C, 0x1D, 0x1F, 0x21, 0x23, 0x25,
              0x26, 0x27, 0x29, 0x2B, 0x2D, 0x2F, 0x30, 0x32 },
            // 8
            { 0x00, 0x01, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0D,
              0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14, 0x17, 0x1A,
              0x19, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x25, 0x28,
              0x29, 0x2A, 0x2B, 0x2D, 0x2F, 0x31, 0x33, 0x35 },
            // 9
            { 0x00, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0E,
              0x0F, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1B,
              0x1C, 0x1D, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x29,
              0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x39 },
            // 10
            { 0x00, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0E,
              0x0F, 0x10, 0x12, 0x14, 0x16, 0x19, 0x1B, 0x1E,
              0x1F, 0x21, 0x23, 0x25, 0x27, 0x29, 0x2B, 0x2D,
              0x2E, 0x2F, 0x31, 0x32, 0x34, 0x36, 0x39, 0x3C },
            // 11
            { 0x00, 0x01, 0x03, 0x05, 0x07, 0x0A, 0x0C, 0x0F,
              0x10, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1E,
              0x1F, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2B, 0x2E,
              0x2F, 0x30, 0x32, 0x34, 0x36, 0x39, 0x3C, 0x3F },
            // 12
            { 0x00, 0x02, 0x04, 0x06, 0x08, 0x0B, 0x0D, 0x10,
              0x11, 0x12, 0x14, 0x16, 0x18, 0x1B, 0x1E, 0x21,
              0x22, 0x23, 0x25, 0x27, 0x29, 0x2C, 0x2F, 0x32,
              0x33, 0x34, 0x36, 0x38, 0x3B, 0x34, 0x41, 0x44 },
            // 13
            { 0x00, 0x02, 0x04, 0x06, 0x08, 0x0B, 0x0D, 0x11,
              0x12, 0x13, 0x15, 0x17, 0x1A, 0x1D, 0x20, 0x23,
              0x24, 0x25, 0x27, 0x29, 0x2C, 0x2F, 0x32, 0x35,
              0x36, 0x37, 0x39, 0x3B, 0x3E, 0x41, 0x44, 0x47 }
        };
    }
}
