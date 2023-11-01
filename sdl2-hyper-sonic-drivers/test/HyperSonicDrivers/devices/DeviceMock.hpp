#pragma once

#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>


namespace HyperSonicDrivers::devices
{
    template<class T>
    class DeviceMock : public T
    {
    public:
        explicit DeviceMock(const std::shared_ptr<audio::IMixer>& mixer) : T(mixer) {
            static_assert(std::is_base_of_v<IDevice, T>);
        };
        DeviceMock() : T(audio::make_mixer<audio::IMixerMock>()) {
            static_assert(std::is_base_of_v<IDevice, T>);
        };

        bool init() noexcept  override { return true; };
        bool shutdown() noexcept override { return true; };
        std::optional<uint8_t> getChannelId() const noexcept { return std::nullopt; };
    };
}

