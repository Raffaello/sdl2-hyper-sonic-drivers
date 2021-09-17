#pragma once

#include <files/miles/XMIFile.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <cstdint>

namespace drivers
{
    namespace miles
    {
        // Test class/experimental to understand midi events

        class XMIParser
        {
        public:
            XMIParser(std::shared_ptr<files::miles::XMIFile> xmi_file, std::shared_ptr<audio::scummvm::Mixer> mixer);
            virtual ~XMIParser();

            //void processTrack(const files::MIDFile::MIDI_track_t& track, const int i);
            //void incTicks();
            void display();
        private:
            std::shared_ptr<files::miles::XMIFile> _xmi_file;
            std::shared_ptr<audio::scummvm::Mixer> _mixer;

            int num_tracks;
            //int division;
            uint32_t tempo;
        };
    }
}
