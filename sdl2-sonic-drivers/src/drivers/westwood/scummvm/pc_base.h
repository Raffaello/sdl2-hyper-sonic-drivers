/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */


#ifndef KYRA_SOUND_PCDRIVER_H
#define KYRA_SOUND_PCDRIVER_H

#include <audio/scummvm/Mixer.hpp>
//#include "kyra/resource/resource.h"
#include <utils/endianness.hpp>
#include <hardware/opl/OPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <mutex>
#include <utils/algorithms.hpp>
#include <hardware/opl/scummvm/dosbox/dosbox.hpp>
#include <hardware/opl/scummvm/Config.hpp>

namespace Kyra {
	typedef unsigned int uint;
	typedef uint8_t uint8;
	typedef uint32_t uint32;
	typedef uint16_t uint16;
	typedef int32_t int32;
	typedef int16_t int16;
	typedef uint8_t byte;

class PCSoundDriver {
public:
	PCSoundDriver() : _soundData(0), _soundDataSize(0) {}
	virtual ~PCSoundDriver() {}

	virtual void initDriver() = 0;
	virtual void setSoundData(uint8_t *data, uint32_t size) = 0;
	virtual void startSound(int track, int volume) = 0;
	virtual bool isChannelPlaying(int channel) const = 0;
	virtual void stopAllChannels() = 0;

	virtual int getSoundTrigger() const { return 0; }
	virtual void resetSoundTrigger() {}

	virtual void setMusicVolume(uint8_t volume) = 0;
	virtual void setSfxVolume(uint8_t volume) = 0;

	// AdLiB (Kyra 1) specific
	virtual void setSyncJumpMask(uint16_t) {};

protected:
	uint8 *getProgram(int progId) {
		// Safety check: invalid progId would crash.
		if (progId < 0 || progId >= (int32)_soundDataSize / 2)
			return nullptr;

		const uint16 offset = utils::READ_LE_UINT16(_soundData + 2 * progId);

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
		} else {
			return _soundData + offset;
		}
	}

	uint8 *_soundData;
	uint32 _soundDataSize;

public:
	//static PCSoundDriver *createAdLib(Audio::Mixer *mixer, int version);
};

class AdLibDriver : public PCSoundDriver {
public:
	AdLibDriver(std::shared_ptr<audio::scummvm::Mixer> mixer, int version, std::shared_ptr< hardware::opl::OPL> opl);
	~AdLibDriver() override;

	void initDriver() override;
	void setSoundData(uint8_t* data, uint32_t size) override;
	void startSound(int track, int volume) override;
	bool isChannelPlaying(int channel) const override;
	void stopAllChannels() override;
	int getSoundTrigger() const override { return _soundTrigger; }
	void resetSoundTrigger() override { _soundTrigger = 0; }

	void callback();

	void setSyncJumpMask(uint16_t mask) override { _syncJumpMask = mask; }

	void setMusicVolume(uint8_t volume) override;
	void setSfxVolume(uint8_t volume) override;

	void play(uint8 track, uint8 volume);
	void internalLoadFile(const char* file);
	void haltTrack();
private:
	int _version;
	//kType _type;
	uint8 _trackEntries[500];
	uint8* _soundDataPtr;
	int _sfxPlayingSound;

	struct Channel {
		bool lock;	// New to ScummVM
		uint8_t opExtraLevel2;
		const uint8_t* dataptr;
		uint8_t duration;
		uint8_t repeatCounter;
		int8_t baseOctave;
		uint8_t priority;
		uint8_t dataptrStackPos;
		const uint8_t* dataptrStack[4];
		int8_t baseNote;
		uint8_t slideTempo;
		uint8_t slideTimer;
		int16_t slideStep;
		int16_t vibratoStep;
		uint8_t vibratoStepRange;
		uint8_t vibratoStepsCountdown;
		uint8_t vibratoNumSteps;
		uint8_t vibratoDelay;
		uint8_t vibratoTempo;
		uint8_t vibratoTimer;
		uint8_t vibratoDelayCountdown;
		uint8_t opExtraLevel1;
		uint8_t spacing2;
		uint8_t baseFreq;
		uint8_t tempo;
		uint8_t timer;
		uint8_t regAx;
		uint8_t regBx;
		typedef void (AdLibDriver::* Callback)(Channel&);
		Callback primaryEffect;
		Callback secondaryEffect;
		uint8_t fractionalSpacing;
		uint8_t opLevel1;
		uint8_t opLevel2;
		uint8_t opExtraLevel3;
		uint8_t twoChan;
		uint8_t unk39;
		uint8_t unk40;
		uint8_t spacing1;
		uint8_t durationRandomness;
		uint8_t secondaryEffectTempo;
		uint8_t secondaryEffectTimer;
		int8_t secondaryEffectSize;
		int8_t secondaryEffectPos;
		uint8_t secondaryEffectRegbase;
		uint16 secondaryEffectData;
		uint8_t tempoReset;
		uint8_t rawNote;
		int8_t pitchBend;
		uint8_t volumeModifier;
	};

	void primaryEffectSlide(Channel& channel);
	void primaryEffectVibrato(Channel& channel);
	void secondaryEffect1(Channel& channel);

	void resetAdLibState();
	void writeOPL(byte reg, byte val);
	void initChannel(Channel& channel);
	void noteOff(Channel& channel);
	void initAdlibChannel(uint8_t num);

	uint16 getRandomNr();
	void setupDuration(uint8_t duration, Channel& channel);

	void setupNote(uint8_t rawNote, Channel& channel, bool flag = false);
	void setupInstrument(uint8_t regOffset, const uint8_t* dataptr, Channel& channel);
	void noteOn(Channel& channel);

	void adjustVolume(Channel& channel);

	uint8_t calculateOpLevel1(Channel& channel);
	uint8_t calculateOpLevel2(Channel& channel);

	static uint16 checkValue(int16_t val) { return utils::CLIP<int16_t>(val, 0, 0x3F); }

	// The driver uses timer/tempo pairs in several places. On every
	// callback, the tempo is added to the timer. This will frequently
	// cause the timer to "wrap around", which is the signal to go ahead
	// and do more stuff.
	static bool advance(uint8_t& timer, uint8_t tempo) {
		uint8_t old = timer;
		timer += tempo;
		return timer < old;
	}

	const uint8_t* checkDataOffset(const uint8_t* ptr, long n) {
		if (ptr) {
			long offset = ptr - _soundData;
			if (n >= -offset && n <= (long)_soundDataSize - offset)
				return ptr + n;
		}
		return nullptr;
	}

	// The sound data has two lookup tables:
	// * One for programs, starting at offset 0.
	// * One for instruments, starting at offset 300, 500, or 1000.
	const uint8_t* getInstrument(int instrumentId) {
		return getProgram(_numPrograms + instrumentId);
	}

	void setupPrograms();
	void executePrograms();

	struct ParserOpcode {
		typedef int (AdLibDriver::* POpcode)(Channel& channel, const uint8_t* values);
		POpcode function;
		const char* name;
		int values;
	};

	static const ParserOpcode _parserOpcodeTable[];
	static const int _parserOpcodeTableSize;

	int update_setRepeat(Channel& channel, const uint8_t* values);
	int update_checkRepeat(Channel& channel, const uint8_t* values);
	int update_setupProgram(Channel& channel, const uint8_t* values);
	int update_setNoteSpacing(Channel& channel, const uint8_t* values);
	int update_jump(Channel& channel, const uint8_t* values);
	int update_jumpToSubroutine(Channel& channel, const uint8_t* values);
	int update_returnFromSubroutine(Channel& channel, const uint8_t* values);
	int update_setBaseOctave(Channel& channel, const uint8_t* values);
	int update_stopChannel(Channel& channel, const uint8_t* values);
	int update_playRest(Channel& channel, const uint8_t* values);
	int update_writeAdLib(Channel& channel, const uint8_t* values);
	int update_setupNoteAndDuration(Channel& channel, const uint8_t* values);
	int update_setBaseNote(Channel& channel, const uint8_t* values);
	int update_setupSecondaryEffect1(Channel& channel, const uint8_t* values);
	int update_stopOtherChannel(Channel& channel, const uint8_t* values);
	int update_waitForEndOfProgram(Channel& channel, const uint8_t* values);
	int update_setupInstrument(Channel& channel, const uint8_t* values);
	int update_setupPrimaryEffectSlide(Channel& channel, const uint8_t* values);
	int update_removePrimaryEffectSlide(Channel& channel, const uint8_t* values);
	int update_setBaseFreq(Channel& channel, const uint8_t* values);
	int update_setupPrimaryEffectVibrato(Channel& channel, const uint8_t* values);
	int update_setPriority(Channel& channel, const uint8_t* values);
	int update_setBeat(Channel& channel, const uint8_t* values);
	int update_waitForNextBeat(Channel& channel, const uint8_t* values);
	int update_setExtraLevel1(Channel& channel, const uint8_t* values);
	int update_setupDuration(Channel& channel, const uint8_t* values);
	int update_playNote(Channel& channel, const uint8_t* values);
	int update_setFractionalNoteSpacing(Channel& channel, const uint8_t* values);
	int update_setTempo(Channel& channel, const uint8_t* values);
	int update_removeSecondaryEffect1(Channel& channel, const uint8_t* values);
	int update_setChannelTempo(Channel& channel, const uint8_t* values);
	int update_setExtraLevel3(Channel& channel, const uint8_t* values);
	int update_setExtraLevel2(Channel& channel, const uint8_t* values);
	int update_changeExtraLevel2(Channel& channel, const uint8_t* values);
	int update_setAMDepth(Channel& channel, const uint8_t* values);
	int update_setVibratoDepth(Channel& channel, const uint8_t* values);
	int update_changeExtraLevel1(Channel& channel, const uint8_t* values);
	int update_clearChannel(Channel& channel, const uint8_t* values);
	int update_changeNoteRandomly(Channel& channel, const uint8_t* values);
	int update_removePrimaryEffectVibrato(Channel& channel, const uint8_t* values);
	int update_pitchBend(Channel& channel, const uint8_t* values);
	int update_resetToGlobalTempo(Channel& channel, const uint8_t* values);
	int update_nop(Channel& channel, const uint8_t* values);
	int update_setDurationRandomness(Channel& channel, const uint8_t* values);
	int update_changeChannelTempo(Channel& channel, const uint8_t* values);
	int updateCallback46(Channel& channel, const uint8_t* values);
	int update_setupRhythmSection(Channel& channel, const uint8_t* values);
	int update_playRhythmSection(Channel& channel, const uint8_t* values);
	int update_removeRhythmSection(Channel& channel, const uint8_t* values);
	int update_setRhythmLevel2(Channel& channel, const uint8_t* values);
	int update_changeRhythmLevel1(Channel& channel, const uint8_t* values);
	int update_setRhythmLevel1(Channel& channel, const uint8_t* values);
	int update_setSoundTrigger(Channel& channel, const uint8_t* values);
	int update_setTempoReset(Channel& channel, const uint8_t* values);
	int updateCallback56(Channel& channel, const uint8_t* values);

private:
	int _curChannel;
	uint8_t _soundTrigger;

	uint16 _rnd;

	uint8_t _beatDivider;
	uint8_t _beatDivCnt;
	uint8_t _callbackTimer;
	uint8_t _beatCounter;
	uint8_t _beatWaiting;
	uint8_t _opLevelBD;
	uint8_t _opLevelHH;
	uint8_t _opLevelSD;
	uint8_t _opLevelTT;
	uint8_t _opLevelCY;
	uint8_t _opExtraLevel1HH;
	uint8_t _opExtraLevel2HH;
	uint8_t _opExtraLevel1CY;
	uint8_t _opExtraLevel2CY;
	uint8_t _opExtraLevel2TT;
	uint8_t _opExtraLevel1TT;
	uint8_t _opExtraLevel1SD;
	uint8_t _opExtraLevel2SD;
	uint8_t _opExtraLevel1BD;
	uint8_t _opExtraLevel2BD;

	hardware::opl::OPL* _adlib;

	struct QueueEntry {
		QueueEntry() : data(0), id(0), volume(0) {}
		QueueEntry(uint8_t* ptr, uint8_t track, uint8_t vol) : data(ptr), id(track), volume(vol) {}
		uint8_t* data;
		uint8_t id;
		uint8_t volume;
	};

	QueueEntry _programQueue[16];
	int _programStartTimeout;
	int _programQueueStart, _programQueueEnd;
	bool _retrySounds;

	void adjustSfxData(uint8_t* data, int volume);
	uint8_t* _sfxPointer;
	int _sfxPriority;
	int _sfxVelocity;

	Channel _channels[10];

	uint8_t _vibratoAndAMDepthBits;
	uint8_t _rhythmSectionBits;

	uint8_t _curRegOffset;
	uint8_t _tempo;

	const uint8_t* _tablePtr1;
	const uint8_t* _tablePtr2;

	static const uint8_t _regOffset[];
	static const uint16 _freqTable[];
	static const uint8_t* const _unkTable2[];
	static const int _unkTable2Size;
	static const uint8_t _unkTable2_1[];
	static const uint8_t _unkTable2_2[];
	static const uint8_t _unkTable2_3[];
	static const uint8_t _pitchBendTables[][32];

	uint16 _syncJumpMask;

	//Common::Mutex _mutex;
	audio::scummvm::Mixer* _mixer;

	uint8_t _musicVolume, _sfxVolume;

	int _numPrograms;
	//int _version;
};

} // End of namespace Kyra

#endif
