#pragma once

#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/devices/IDevice.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <type_traits>

namespace HyperSonicDrivers::devices
{
    template<class T>
    class SpyDevice : public T
    {
    public:
        SpyDevice(const std::shared_ptr<audio::IMixer>& mixer) : T(mixer) {
            static_assert(std::is_base_of_v<IDevice, T>);
        };
        SpyDevice() : T(audio::make_mixer<audio::stubs::StubMixer>()) {
            static_assert(std::is_base_of_v<IDevice, T>);
        };

        bool init() noexcept  override { return true; };
        bool shutdown() noexcept override { return true; };
        std::optional<uint8_t> getChannelId() const noexcept { return std::nullopt; };


       /* virtual void sendEvent(const audio::midi::MIDIEvent& e) const noexcept override
        {

        };
        virtual void sendMessage(const uint8_t msg[], const uint8_t size) const noexcept override
        {

        }
        virtual void sendSysEx(const audio::midi::MIDIEvent& e) const noexcept override
        {

        }*/
        /*virtual void pause() const noexcept override
        {

        }
        virtual void resume() const noexcept override
        {

        }*/
    };
}

