#include <drivers/MIDParser.hpp>
#include <spdlog/spdlog.h>

namespace drivers
{
    using files::MIDFile;

    MIDParser::MIDParser(std::shared_ptr<files::MIDFile> mid_file, std::shared_ptr<audio::scummvm::Mixer> mixer)
        : _mid_file(mid_file), _mixer(mixer)
    {
    }

    MIDParser::~MIDParser()
    {
    }

    void MIDParser::display()
    {
        if (_mid_file->getFormat() == 2) {
            spdlog::info("MIDI format 2 not supported");
            return;
        }


    }
}
