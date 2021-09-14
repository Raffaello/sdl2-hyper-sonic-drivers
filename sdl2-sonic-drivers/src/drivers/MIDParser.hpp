#pragma once

#include <audio/scummvm/Mixer.hpp>
#include <files/MIDFile.hpp>
#include <memory>

namespace drivers
{
    // Test class/experimental to understand midi events
    class MIDParser
    {
    public:
        MIDParser(std::shared_ptr<files::MIDFile> mid_file, std::shared_ptr<audio::scummvm::Mixer> mixer);
        virtual ~MIDParser();

        void display();
    private:
        std::shared_ptr<files::MIDFile> _mid_file;
        std::shared_ptr<audio::scummvm::Mixer> _mixer;
    };
}
