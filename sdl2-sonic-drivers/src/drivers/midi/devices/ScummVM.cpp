#include <drivers/midi/devices/ScummVM.hpp>
#include <audio/midi/types.hpp>
#include <spdlog/spdlog.h>
#include <cassert>

namespace drivers
{
    namespace midi
    {
        namespace devices
        {
            ScummVM::ScummVM(std::shared_ptr<hardware::opl::OPL> opl, const bool opl3mode) : Device()
            {
                _adlib = std::make_shared<drivers::midi::scummvm::MidiDriver_ADLIB>(opl, opl3mode);
                _adlib->open();
            }

            ScummVM::~ScummVM()
            {
                _adlib->close();
            }

            inline void ScummVM::sendEvent(const audio::midi::MIDIEvent& e) const noexcept
            {
                uint32_t b = e.type.val + (e.data[0]<<8);
                if (e.data.size() == 2)
                    b += (e.data[1] << 16);
                _adlib->send(b);
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="msg">Always 3 size, if it is 2 in size, 3rd value must be zero</param>
            /// <param name="size"></param>
            /// <returns></returns>
            inline void ScummVM::sendMessage(const uint8_t msg[], const uint8_t size) const noexcept
            {
                assert(size >= 2 && size <= 3);
                uint32_t b = msg[0] + (msg[1] << 8);
                // the if below should be commented out
                //if (size == 3)
                    b += (msg[2] << 16);
                _adlib->send(b);
            }


#include<drivers/midi/scummvm/AdLibInstrument.h> // TODO remove the include just for testing
            void ScummVM::sendSysEx(const audio::midi::MIDIEvent& e) const noexcept
            {
                // NOTE: TODO: here is replacing an instrument and it should work but it doesnt.
                //             it looks like the OP2 format is not compatible with imuse adlib instr format
                // TODO: Must do a simple MIDI Adlib driver 
                //static int chan = 0;
                // expected that the data has 4 byte type, 1 byte channel, and the adlib instrument data struct
                // TODO: TEST
                //drivers::midi::scummvm::AdLibInstrument adlib;
                //if (chan == 0) {
                    /*
                adlibWrite(channel + 0x20, instr->modCharacteristic);
                adlibWrite(channel + 0x40, (instr->modScalingOutputLevel | 0x3F) - vol1);
                adlibWrite(channel + 0x60, 0xff & (~instr->modAttackDecay));
                adlibWrite(channel + 0x80, 0xff & (~instr->modSustainRelease));
                adlibWrite(channel + 0xE0, instr->modWaveformSelect);

                channel = g_operator2Offsets[chan];
                adlibWrite(channel + 0x20, instr->carCharacteristic);
                adlibWrite(channel + 0x40, (instr->carScalingOutputLevel | 0x3F) - vol2);
                adlibWrite(channel + 0x60, 0xff & (~instr->carAttackDecay));
                adlibWrite(channel + 0x80, 0xff & (~instr->carSustainRelease));
                adlibWrite(channel + 0xE0, instr->carWaveformSelect);
                    * 
                    */
                //    adlib.modCharacteristic = 0x10;
                //    adlib.modScalingOutputLevel = 0x0;
                //    adlib.modAttackDecay = 241;
                //    adlib.modSustainRelease = 255;
                //    adlib.modWaveformSelect = 0;
                //    adlib.carCharacteristic = 81;
                //    adlib.carScalingOutputLevel = 0;
                //    adlib.carAttackDecay = 240;
                //    adlib.carSustainRelease = 255;
                //    adlib.carWaveformSelect = 1;
                //    adlib.feedback = 12;
                //    adlib.flagsA = 0;
                //    adlib.extraA = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.flagsB = 0;
                //    adlib.extraB = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.duration = 0; // instr.voices[0].noteOffset;
                //}
                //else if (chan==1) {
                //    adlib.modCharacteristic = 0x10;
                //    adlib.modScalingOutputLevel = 0x0;
                //    adlib.modAttackDecay = 241;
                //    adlib.modSustainRelease = 255;
                //    adlib.modWaveformSelect = 0;
                //    adlib.carCharacteristic = 81;
                //    adlib.carScalingOutputLevel = 0;
                //    adlib.carAttackDecay = 240;
                //    adlib.carSustainRelease = 255;
                //    adlib.carWaveformSelect = 1;
                //    adlib.feedback = 2;
                //    adlib.flagsA = 0;
                //    adlib.extraA = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.flagsB = 0;
                //    adlib.extraB = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.duration = 0; // instr.voices[0].noteOffset;
                //}
                //else {
                //    // 30
                //    //adlib = { 0xC2, 0x30, 0x4F, 0xCA, 0x01, 0xC4, 0x0D, 0x0E, 0xB8, 0x7F, 0x08, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, 0x00 };

                //    adlib.modCharacteristic = 0xC2;
                //    adlib.modScalingOutputLevel = 0x30;
                //    adlib.modAttackDecay = 0x4F;
                //    adlib.modSustainRelease = 0xCA;
                //    adlib.modWaveformSelect = 1;
                //    adlib.carCharacteristic = 0xC4;
                //    adlib.carScalingOutputLevel = 0x0D;
                //    adlib.carAttackDecay = 0x0E;
                //    adlib.carSustainRelease = 0xB8;
                //    adlib.carWaveformSelect = 0x7F;
                //    adlib.feedback =0x08;
                //    adlib.flagsA = 0;
                //    adlib.extraA = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.flagsB = 0;
                //    adlib.extraB = { 0, 0, 0, 0, 0, 0, 0, 0 };
                //    adlib.duration = 0; // instr.voices[0].noteOffset;
                //}

                using audio::midi::MIDI_EVENT_TYPES_HIGH;
                using audio::midi::MIDI_META_EVENT_TYPES_LOW;

                if (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high) == MIDI_EVENT_TYPES_HIGH::META_SYSEX &&
                    static_cast<MIDI_META_EVENT_TYPES_LOW>(e.type.low) == MIDI_META_EVENT_TYPES_LOW::SYS_EX0)
                {
                    //if (e.data.size() != (sizeof(uint32_t) + sizeof(OP2File::instrument_t)) {
                    //    // data size not valid ...
                    //}

                    // extract sysEx type
                    // TODO: do based on endianness (now Big Endian) 
                    uint32_t type = e.data[3] + (e.data[2] << 8) + (e.data[1] << 16) + (e.data[0] << 24);
                    uint8_t channel = e.data[4];
                    _adlib->sysEx_customInstrument(channel, type, &(e.data.data()[5]));

                    
                }
                else
                {
                    // it should not receive a non META SysEx0 message here
                    spdlog::warn("sensSysEx received a non META SysEx MIDI event");
                    return;
                }

                //_adlib->sysEx_customInstrument(chan, 'OP2 ', reinterpret_cast<uint8_t*>(& adlib));
                //chan++;
            }
        }
    }
}
