#pragma once

namespace HyperSonicDrivers::drivers
{
    /**
    * Common Interface for music drivers: MIDI, ADL, XMI, MUS...
    * 
    **/
    class IMusicDriver
    {
    public:
        IMusicDriver(IMusicDriver&) = delete;
        IMusicDriver(IMusicDriver&&) = delete;
        IMusicDriver& operator=(IMusicDriver&) = delete;

        IMusicDriver() = default;
        virtual ~IMusicDriver() = default;

        virtual void play(const uint8_t track) noexcept = 0;
        virtual void stop() noexcept = 0;

        // TODO: it might not be required
        //virtual void pause() = 0;
        // TODO: it might not be required
        //virtual void resume() = 0;

        virtual bool isPlaying() const noexcept = 0;

        // TODO: it might not be required
        //virtual bool isPaused() const noexcept = 0;
    };
}
