#pragma once

#include <drivers/midi/scummvm/MidiDriver.hpp>
#include <drivers/midi/scummvm/MidiChannel.hpp>
#include <drivers/midi/scummvm/AdLibInstrument.h>
#include <drivers/midi/scummvm/AdlibVoice.h>
#include <drivers/midi/scummvm/AdLibPart.hpp>
#include <drivers/midi/scummvm/AdLibPercussionChannel.hpp>

#include <hardware/opl/OPL.hpp>

#include <cstdint>
#include <memory>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {
            struct AdLibSetParams {
                uint8_t registerBase;
                uint8_t shift;
                uint8_t mask;
                uint8_t inversion;
            };

            class MidiDriver_ADLIB : public MidiDriver
            {
                friend class AdLibPart;
                friend class AdLibPercussionChannel;

            public:
                MidiDriver_ADLIB(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode);

                int open() override;
                void close() override;
                void send(uint32_t b) override;
                void send(int8_t channel, uint32_t b) override; // Supports higher than channel 15
                uint32_t property(int prop, uint32_t param) override;
                bool isOpen() const override { return _isOpen; }
                uint32_t getBaseTempo() override { return 1000000 / hardware::opl::DEFAULT_CALLBACK_FREQUENCY; }

                void setPitchBendRange(uint8_t channel, unsigned int range) override;
                void sysEx_customInstrument(uint8_t channel, uint32_t type, const uint8_t* instr) override;

                MidiChannel* allocateChannel() override;
                MidiChannel* getPercussionChannel() override { return &_percussion; } // Percussion partially supported

                virtual void setTimerCallback(void* timerParam, /*Common::TimerManager::TimerProc*/ void* timerProc);

            private:
                bool _scummSmallHeader; // FIXME: This flag controls a special mode for SCUMM V3 games
                bool _opl3Mode;

                std::shared_ptr<hardware::opl::OPL> _opl;
                uint8_t* _regCache;
                uint8_t* _regCacheSecondary;

                /*Common::TimerManager::TimerProc*/ void* _adlibTimerProc;
                void* _adlibTimerParam;

                int _timerCounter;

                uint16_t _channelTable2[9];
                int _voiceIndex;
                int _timerIncrease;
                int _timerThreshold;
                uint16_t _curNotTable[9];
                AdLibVoice _voices[9];
                AdLibPart _parts[32];
                AdLibPercussionChannel _percussion;

                bool _isOpen;

                void onTimer();
                void partKeyOn(AdLibPart* part, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan);
                void partKeyOff(AdLibPart* part, uint8_t note);

                void adlibKeyOff(int chan);
                void adlibNoteOn(int chan, uint8_t note, int mod);
                void adlibNoteOnEx(int chan, uint8_t note, int mod);
                int adlibGetRegValueParam(int chan, uint8_t data);
                void adlibSetupChannel(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2);
                void adlibSetupChannelSecondary(int chan, const AdLibInstrument* instr, uint8_t vol1, uint8_t vol2, uint8_t pan);
                uint8_t adlibGetRegValue(uint8_t reg) {
                    return _regCache[reg];
                }
                uint8_t adlibGetRegValueSecondary(uint8_t reg) {
                    return _regCacheSecondary[reg];
                }
                void adlibSetParam(int channel, uint8_t param, int value, bool primary = true);
                void adlibKeyOnOff(int channel);
                void adlibWrite(uint8_t reg, uint8_t value);
                void adlibWriteSecondary(uint8_t reg, uint8_t value);
                void adlibPlayNote(int channel, int note);

                AdLibVoice* allocateVoice(uint8_t pri);

                void mcOff(AdLibVoice* voice);

                static void linkMc(AdLibPart* part, AdLibVoice* voice);
                void mcIncStuff(AdLibVoice* voice, Struct10* s10, Struct11* s11);
                void mcInitStuff(AdLibVoice* voice, Struct10* s10, Struct11* s11, uint8_t flags,
                    const InstrumentExtra* ie);

                void struct10Init(Struct10* s10, const InstrumentExtra* ie);
                static uint8_t struct10OnTimer(Struct10* s10, Struct11* s11);
                static void struct10Setup(Struct10* s10);
                static int randomNr(int a);
                void mcKeyOn(AdLibVoice* voice, const AdLibInstrument* instr, uint8_t note, uint8_t velocity, const AdLibInstrument* second, uint8_t pan);
            };
        }
    }
}
