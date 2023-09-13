#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    constexpr int default_opl_callback_freq = 250;

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

        inline bool isInit() const noexcept { return m_init; }

        inline bool isStereo() const noexcept { return type != OplType::OPL2; }

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
        //void start(const std::shared_ptr<TimerCallBack>& callback, int timerFrequency = default_opl_callback_freq);
        void start(
            const std::shared_ptr<TimerCallBack>& callback,
            const audio::mixer::eChannelGroup group = audio::mixer::eChannelGroup::Plain,
            const uint8_t volume = 255,
            const uint8_t pan = 0,
            int timerFrequency = default_opl_callback_freq);

        /**
         * Stop the OPL
         */
        void stop();

        /**
         * Change the callback frequency. This must only be called from a
         * timer proc.
         */
        virtual uint32_t setCallbackFrequency(const int timerFrequency) = 0;

    protected:
        bool m_init = false;
        /**
         * Start the callbacks.
         */
        virtual void startCallbacks(
            const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan,
            const int timerFrequency
        ) = 0;

        /**
         * Stop the callbacks.
         */
        virtual void stopCallbacks() = 0;

        /**
         * The functor for callbacks.
         */
        std::shared_ptr<TimerCallBack> m_callback;
    private:
        // moved into cpp file
        //static bool _hasInstance;
    };
}
