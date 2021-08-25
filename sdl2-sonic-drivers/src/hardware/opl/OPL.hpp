#pragma once

#include <cstdint>
#include <memory>
#include <functional>

namespace hardware
{
    namespace opl
    {
        constexpr int DEFAULT_CALLBACK_FREQUENCY = 250;

        typedef std::function<void()> TimerCallBack;

        /**
         * A representation of a Yamaha OPL chip.
         */
        class OPL
        {
        public:
            OPL();
            virtual ~OPL();

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
             *
             * @param a port address
             * @param v value, which will be written
             */
            virtual void write(int a, int v) = 0;

            /**
             * Reads a byte from the given I/O port.
             *
             * @param a port address
             * @return value read
             */
            virtual uint8_t read(int a) = 0;

            /**
             * Function to directly write to a specific OPL register.
             * This writes to *both* chips for a Dual OPL2. We allow
             * writing to secondary OPL registers by using register
             * values >= 0x100.
             *
             * @param r		hardware register number to write to
             * @param v		value, which will be written
             */
            virtual void writeReg(int r, int v) = 0;

            /**
             * Start the OPL with callbacks.
             */
            void start(TimerCallBack* callback, int timerFrequency = DEFAULT_CALLBACK_FREQUENCY);

            /**
             * Stop the OPL
             */
            void stop();

            /**
             * Change the callback frequency. This must only be called from a
             * timer proc.
             */
            virtual void setCallbackFrequency(int timerFrequency) = 0;

        protected:
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
            std::unique_ptr<TimerCallBack> _callback;
        private:
            // moved into cpp file
            //static bool _hasInstance;
        };
    }
}