#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <files/MIDFile.hpp>
#include <memory>
#include <cstdint>

namespace drivers
{
    // Test class/experimental to understand midi events
    class MIDParser
    {
    public:
        MIDParser(std::shared_ptr<files::MIDFile> mid_file, std::shared_ptr<audio::scummvm::Mixer> mixer);
        virtual ~MIDParser();

        void processTrack(const files::MIDFile::MIDI_track_t& track, const int i);
        void display();
    private:
        std::shared_ptr<files::MIDFile> _mid_file;
        std::shared_ptr<audio::scummvm::Mixer> _mixer;

        int num_tracks;
        int division;
        uint32_t tempo;
    };
}
