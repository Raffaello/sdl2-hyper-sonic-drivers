#include <files/dmx/MUSFile.hpp>

namespace files
{
    namespace dmx
    {
        MUSFile::MUSFile(const std::string& filename) : File(filename)
        {
            // reading similar to a standard midi.

            // now due the MUS compatibility checking:
            // 1. using 9 channels.
            // 2. files size max 64KB
            // 3. format of size is 16 bit (probably i can read from midifile directly then as it is uses 32bits)
            // 

        }
    }
}
