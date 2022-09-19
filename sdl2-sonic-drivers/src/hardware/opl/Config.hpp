#pragma once

#include <cstdint>
#include <string>
#include <hardware/opl/OPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>
#include <hardware/opl/OplEmulator.hpp>
#include <hardware/opl/OplType.hpp>

namespace hardware
{
    namespace opl
    {
        /**
         * @defgroup audio_fmopl OPL emulation
         * @ingroup audio
         *
         * @brief OPL class for managing an OPS emulator.
         * @{
         */
        class Config
        {
        public:
            /**
             * Creates the specific driver with a specific type setup.
             */
            static std::shared_ptr<OPL> create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
        };
    }
}
