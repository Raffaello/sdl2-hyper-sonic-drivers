#pragma once

namespace HyperSonicDrivers::drivers
{
    /**
    * Common Interface for music drivers: MIDI, ADL, XMI, MUS...
    * TODO/Rename: if not merged with IMidiDriver, this can be renamed as IMusicPlayer
    *              related to playing those files rather then using a driver,
    *              ADLDriver at the moment is both
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
