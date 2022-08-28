#include <drivers/midi/adlib/MidiDriver.hpp>
#include <spdlog/spdlog.h>

namespace drivers
{
    namespace midi
    {
        namespace adlib
        {
            constexpr uint8_t NUM_CHANNELS = 9; // OPL2

            MidiDriver::MidiDriver(std::shared_ptr<hardware::opl::OPL> opl) : _opl(opl)
            {
                init();
            }

            MidiDriver::~MidiDriver()
            {
                // deinit
                stopAll();
                _opl->writeReg(0x01, 0x20); // enable Waveform Select
                _opl->writeReg(0x08, 0x00); // turn off CSW mode
                _opl->writeReg(0xBD, 0x00); // set vibrato/tremolo depth to low, set melodic mode
            }

            void MidiDriver::send(const audio::midi::MIDIEvent& e) const noexcept
            {
                using audio::midi::MIDI_EVENT_TYPES_HIGH;

                switch (static_cast<MIDI_EVENT_TYPES_HIGH>(e.type.high))
                {
                case MIDI_EVENT_TYPES_HIGH::NOTE_OFF:
                    break;
                case MIDI_EVENT_TYPES_HIGH::NOTE_ON:
                    break;
                case MIDI_EVENT_TYPES_HIGH::AFTERTOUCH:
                    break;
                case MIDI_EVENT_TYPES_HIGH::CONTROLLER:
                    break;
                case MIDI_EVENT_TYPES_HIGH::PROGRAM_CHANGE:
                    break;
                case MIDI_EVENT_TYPES_HIGH::CHANNEL_AFTERTOUCH:
                    break;
                case MIDI_EVENT_TYPES_HIGH::PITCH_BEND:
                    break;
                case MIDI_EVENT_TYPES_HIGH::META_SYSEX:
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

                // detect OPL2 (not working with emulator, see OPLExample.cpp results)
                /*
                uint8_t stat1, stat2;

                _opl->writeReg(0x04, 0x60);
                _opl->writeReg(0x04, 0x80);
                stat1 = _opl->read(1) & 0xE0; // default to "port = 0x220 | 0x388
                _opl->writeReg(0x02, 0xFF);
                _opl->writeReg(0x04, 0x21);
                for (int i = 512; --i; ) {
                    _opl->read(0);
                }

                stat2 = _opl->read(1) & 0xE0;
                _opl->writeReg(0x04, 0x60);
                _opl->writeReg(0x04, 0x80);
                //OPLport = origPort;

                bool result = (stat1 == 0 && stat2 == 0xC0);
                */


                // Init Adlib
                _opl->writeReg(0x01, 0x20);		// enable Waveform Select
                _opl->writeReg(0x08, 0x40);		// turn off CSW mode
                _opl->writeReg(0xBD, 0x00);		// set vibrato/tremolo depth to low, set melodic mode

                stopAll();


            }

            void MidiDriver::stopAll() const noexcept
            {
                for (int i = 0; i < NUM_CHANNELS; i++)
                {
                    writeChannel(0x40, i, 0x3F, 0x3F);  // turn off volume
                    writeChannel(0x60, i, 0xFF, 0xFF);  // the fastest attack, decay
                    writeChannel(0x80, i, 0x0F, 0x0F);  // ... and release
                    writeValue(0xB0, i, 0);             // KEY-OFF
                }
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

            void MidiDriver::writeInstrument(const uint8_t channel, struct OPL2instrument* instr) const noexcept
            {
                writeChannel(0x40, channel, 0x3F, 0x3F);    // no volume
                writeChannel(0x20, channel, instr->trem_vibr_1, instr->trem_vibr_2);
                writeChannel(0x60, channel, instr->att_dec_1, instr->att_dec_2);
                writeChannel(0x80, channel, instr->sust_rel_1, instr->sust_rel_2);
                writeChannel(0xE0, channel, instr->wave_1, instr->wave_2);
                writeValue(0xC0, channel, instr->feedback | 0x30);
            }
        }
    }
}