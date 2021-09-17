#include "XMIParser.hpp"
#include "XMIParser.hpp"
#include <drivers/miles/XMIParser.hpp>
#include <vector>
#include <spdlog/spdlog.h>

namespace drivers
{
    namespace miles
    {
        XMIParser::XMIParser(std::shared_ptr<files::miles::XMIFile> xmi_file, std::shared_ptr<audio::scummvm::Mixer> mixer)
            : _xmi_file(xmi_file), _mixer(mixer)
        {
            num_tracks = _xmi_file->getNumTracks();
        }

        XMIParser::~XMIParser()
        {
        }

        void XMIParser::display()
        {
            for (int it = 0; it < num_tracks; it++)
            {
                std::vector<uint8_t> track = _xmi_file->getTrack(it);

                // process track
                // TODO need to be processed byte by byte ...
                // BODY: might have sense to have it in XMIFile
                int i = 0;
                switch (track[i++])
                {
                case 0xFF:
                {
                    uint8_t type = track[i++];
                    // decode_xmi_vlq
                    // TODO:
                    spdlog::info("Meta event type={:#04x}", type);
                }
                    break;
                default:
                    spdlog::warn("u={:#04x}", track[i - 1]);
                    return;
                }
            }
        }
    }
}
