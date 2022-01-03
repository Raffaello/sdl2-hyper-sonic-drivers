#include <drivers/midi/scummvm/MidiDriver_BASE.hpp>

namespace drivers
{
    namespace midi
    {
        namespace scummvm
        {

            void MidiDriver_BASE::midiDumpInit() {
                //g_system->displayMessageOnOSD(_("Starting MIDI dump"));
                //_midiDumpCache.clear();
                //_prevMillis = g_system->getMillis(true);
            }

            int MidiDriver_BASE::midiDumpVarLength(const uint32_t& delta) {
                // MIDI file format has a very strange representation - "Variable Length Values"
                // we're using only *7* bits of each uint8_t for the data
                // the MSB bit is 1 for all bytes, except the last one
                //if (delta <= 127) {
                //    // "Variable Length Values" of 1 uint8_t
                //    spdlog::debug("{#02x}", delta);
                //    _midiDumpCache.push_back(delta);
                //    return 1;
                //}
                //else {
                //    // "Variable Length Values" of 2 bytes
                //    // theoretically, "Variable Length Values" can have more than 2 bytes, but it won't happen in our use case
                //    uint8_t msb = delta / 128;
                //    msb |= 0x80;
                //    uint8_t lsb = delta % 128;
                //    spdlog::debug("{#02x},{#02x}", msb, lsb);
                //    _midiDumpCache.push_back(msb);
                //    _midiDumpCache.push_back(lsb);
                //    return 2;
                //}

                return 0;
            }

            void MidiDriver_BASE::midiDumpDelta() {
                //uint32_t millis = g_system->getMillis(true);
                //uint32_t delta = millis - _prevMillis;
                //_prevMillis = millis;

                ////debugN("MIDI : delta(");
                ////int varLength = midiDumpVarLength(delta);
                //if (varLength == 1)
                //    debugN("),\t ");
                //else
                //    debugN("), ");
            }

            void MidiDriver_BASE::midiDumpDo(uint32_t b) {
                /*const uint8_t status = b & 0xff;
                const uint8_t firstOp = (b >> 8) & 0xff;
                const uint8_t secondOp = (b >> 16) & 0xff;

                midiDumpDelta();
                debugN("message(0x%02x 0x%02x", status, firstOp);

                _midiDumpCache.push_back(status);
                _midiDumpCache.push_back(firstOp);

                if (status < 0xc0 || status > 0xdf) {
                    _midiDumpCache.push_back(secondOp);
                    debug(" 0x%02x)", secondOp);
                }
                else
                    debug(")");*/
            }

            void MidiDriver_BASE::midiDumpSysEx(const uint8_t* msg, uint16_t length) {
                //midiDumpDelta();
                //_midiDumpCache.push_back(0xf0);
                //debugN("0xf0, length(");
                //midiDumpVarLength(length + 1);		// +1 because of closing 0xf7
                //debugN("), sysex[");
                //for (int i = 0; i < length; i++) {
                //    debugN("0x%x, ", msg[i]);
                //    _midiDumpCache.push_back(msg[i]);
                //}
                //debug("0xf7]\t\t");
                //_midiDumpCache.push_back(0xf7);
            }


            void MidiDriver_BASE::midiDumpFinish() {
                //Common::DumpFile* midiDumpFile = new Common::DumpFile();
                //midiDumpFile->open("dump.mid");
                //midiDumpFile->write("MThd\0\0\0\x6\0\x1\0\x2", 12);		// standard MIDI file header, with two tracks
                //midiDumpFile->write("\x1\xf4", 2);						// division - 500 ticks per beat, i.e. a quarter note. Each tick is 1ms
                //midiDumpFile->write("MTrk", 4);							// start of first track - doesn't contain real data, it's just common practice to use two tracks
                //midiDumpFile->writeUint32BE(4);							// first track size
                //midiDumpFile->write("\0\xff\x2f\0", 4);			    	// meta event - end of track
                //midiDumpFile->write("MTrk", 4);							// start of second track
                //midiDumpFile->writeUint32BE(_midiDumpCache.size() + 4);	// track size (+4 because of the 'end of track' event)
                //midiDumpFile->write(_midiDumpCache.data(), _midiDumpCache.size());
                //midiDumpFile->write("\0\xff\x2f\0", 4);			    	// meta event - end of track
                //midiDumpFile->finalize();
                //midiDumpFile->close();
                //const char msg[] = "Ending MIDI dump, created 'dump.mid'";
                //g_system->displayMessageOnOSD(_(msg));		//TODO: why it doesn't appear?
                //debug("%s", msg);
            }

            MidiDriver_BASE::MidiDriver_BASE() {
                //_midiDumpEnable = ConfMan.getBool("dump_midi");
                /*if (_midiDumpEnable) {
                    midiDumpInit();
                }*/
            }

            MidiDriver_BASE::~MidiDriver_BASE() {
                /* if (_midiDumpEnable && !_midiDumpCache.empty()) {
                     midiDumpFinish();
                 }*/
            }

            void MidiDriver_BASE::send(uint8_t status, uint8_t firstOp, uint8_t secondOp) {
                send(status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
            }

            void MidiDriver_BASE::send(int8_t source, uint8_t status, uint8_t firstOp, uint8_t secondOp) {
                send(source, status | ((uint32_t)firstOp << 8) | ((uint32_t)secondOp << 16));
            }

            void MidiDriver_BASE::stopAllNotes(bool stopSustainedNotes) {
                for (int i = 0; i < 16; ++i) {
                    send(0xB0 | i, MIDI_CONTROLLER_ALL_NOTES_OFF, 0);
                    if (stopSustainedNotes)
                        send(0xB0 | i, MIDI_CONTROLLER_SUSTAIN, 0); // Also send a sustain off event (bug #5524)
                }
            }

        }
    }
}