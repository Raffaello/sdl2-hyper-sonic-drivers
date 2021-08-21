#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <drivers/westwood/ADLDriver.hpp>

namespace audio
{
    namespace scummvm
    {

            /**
             * AdLib/PC Speaker (early version) implementation of the
             * sound output device.
             *
             * It uses a special sound file format special to EoB I, II,
             * Dune II, Kyrandia 1 and 2 and LoL. EoB I has a slightly
             * different (oldest) file format, EoB II, Dune II and
             * Kyrandia 1 have the exact same format, Kyrandia 2  and
             * LoL have a slightly different format.
             *
             * For PC Speaker this is a little different. Only the EoB
             * games use the old driver with this data file format. The
             * newer games use a MIDI-like driver (see pcspeaker_v2.cpp).
             *
             * See AdLibDriver / PCSpeakerDriver for more information.
             * @see AdLibDriver
             */
            class SoundPC_v1 /* : public Sound */ {
            public:
                SoundPC_v1(scummvm::Mixer* mixer);
                ~SoundPC_v1(); //override;

                //kType getMusicType() const override { return _type; }

                bool init(); // override;
                void process(); // override;

                void updateVolumeSettings(); // override;

                void initAudioResourceInfo(int set, void* info); // override;
                void selectAudioResourceSet(int set); // override;
                //bool hasSoundFile(unsigned int file) const; // override;
                //void loadSoundFile(unsigned int file); // override;
                //void loadSoundFile(Common::String file); // override;

                void playTrack(uint8_t track); // override;
                void haltTrack(); // override;
                bool isPlaying() const; // override;

                //void playSoundEffect(uint16 track, uint8_t volume = 0xFF); // override;

                //void beginFadeOut(); // override;

                //int checkTrigger(); // override;
                //void resetTrigger(); // override;

                static void callback(void* userdata, uint8_t* audiobuf, int len)
                {
                    SoundPC_v1* self = static_cast<SoundPC_v1*>(userdata);

                    self->process();

                    int16_t* buf = reinterpret_cast<int16_t*>(audiobuf);

                    int samples = self->_driver->readBuffer(buf, len / self->getsampsize());
                    int volume = self->getVolume();
                    for (int i = 0; i < samples; i++) {
                        //printf("0x%x\n", buf[i]);
                        buf[i] = static_cast<int16_t>(buf[i] * volume / MIX_MAX_VOLUME);
                    }

                    //self->bJustStartedPlaying = false;
                }
            private:
                //void internalLoadFile(Common::String file);

                void play(uint8_t track, uint8_t volume);

                //const SoundResourceInfo_PC* res() const { return _resInfo[_currentResourceSet]; }
                //SoundResourceInfo_PC* _resInfo[3];
                //int _currentResourceSet;

                drivers::westwood::ADLDriver* _driver;

                int _version;
                //kType _type;
                uint8_t _trackEntries[500];
                uint8_t* _soundDataPtr;
                int _sfxPlayingSound;

                Common::String _soundFileLoaded;

                int _numSoundTriggers;
                const int* _soundTriggers;

                static const int _kyra1NumSoundTriggers;
                static const int _kyra1SoundTriggers[];
            };
    }
}