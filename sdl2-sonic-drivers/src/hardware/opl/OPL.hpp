#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <audio/scummvm/SoundHandle.hpp>
#include <hardware/opl/OplType.hpp>

namespace hardware::opl
{
    constexpr int DEFAULT_CALLBACK_FREQUENCY = 250;

    typedef std::function<void()> TimerCallBack;

    /**
     * A representation of a Yamaha OPL chip.
     */
    class OPL
    {
    public:
        OPL(const OPL&) = delete;
        OPL(const OPL&&) = delete;
        OPL& operator=(const  OPL&) = delete;

        explicit OPL(const OplType type);
        virtual ~OPL();

        const OplType type;

        inline bool isInit() const noexcept
        {
            return _init;
        }

        /**
         * Initializes the OPL emulator.
         *
         * @return true on success, false on failure
         */
        virtual bool init() = 0;

        /**
         * Reinitializes the OPL emulator
         */
        virtual void reset() = 0;

        /**
         * Writes a byte to the given I/O port.
         * @param port
         * @param val value, which will be written (8 bits, 16 due to OPL3 2nd register)
         */
        virtual void write(const uint32_t port, const uint16_t val) noexcept = 0;

        /**
         * Reads a byte from the given I/O port.
         * @return value read
         */
        virtual uint8_t read(const uint32_t port) noexcept = 0;

        /**
         * TODO review this logic of 0x100 2nd register
         *      as it looks ok, but not well implemented for the interface
         *      especially if it is an OPL2
         *
         * Function to directly write to a specific OPL register.
         * This writes to *both* chips for a Dual OPL2. We allow
         * writing to secondary OPL registers by using register
         * values >= 0x100.
         *
         * @param r hardware register number to write to (8 bits, 16 bits for 2nd register in OPL3)
         * @param v value, which will be written (8 bits, 16 bits for consistency with write method)
         */
        virtual void writeReg(const uint16_t r, const uint16_t v) noexcept = 0;

        /**
         * Start the OPL with callbacks.
         */
        void start(const std::shared_ptr<TimerCallBack>& callback, int timerFrequency = DEFAULT_CALLBACK_FREQUENCY);

        /**
         * Stop the OPL
         */
        void stop();

        /**
         * Change the callback frequency. This must only be called from a
         * timer proc.
         */
        virtual void setCallbackFrequency(int timerFrequency) = 0;

        /**
         * get Sound Handle for the mixer, used in Emulated Opl
         * TODO: consider to remove the abastraction of EmulatedOPLs and RealOPLs
         * TOOD: if this is returning this, probably should store the _handle here
         *       instead of EmulatedOPL
         */
        virtual std::shared_ptr<audio::scummvm::SoundHandle> getSoundHandle() const noexcept = 0;

    protected:
        bool _init = false;
        /**
         * Start the callbacks.
         */
        virtual void startCallbacks(int timerFrequency) = 0;

        /**
         * Stop the callbacks.
         */
        virtual void stopCallbacks() = 0;

        /**
         * The functor for callbacks.
         */
        std::shared_ptr<TimerCallBack> _callback;
    private:
        // moved into cpp file
        //static bool _hasInstance;
    };
}
