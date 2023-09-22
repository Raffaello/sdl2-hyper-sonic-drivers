#include <HyperSonicDrivers/devices/IMidiDevice.hpp>
#include <HyperSonicDrivers/devices/midi/MidiMT32.hpp>

namespace HyperSonicDrivers::devices
{
    template<class T, typename... Args>
    std::shared_ptr<devices::IMidiDevice> make_midi_device(Args... args)
    {
        return std::dynamic_pointer_cast<devices::IMidiDevice>(std::make_shared<T>(args...));
    }
}
