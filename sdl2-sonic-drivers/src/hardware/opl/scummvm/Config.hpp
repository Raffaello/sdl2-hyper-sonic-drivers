#pragma once

#include <cstdint>
#include <string>
#include <hardware/opl/OPL.hpp>
#include <audio/scummvm/Mixer.hpp>
#include <memory>

namespace hardware
{
    namespace opl
    {
        namespace scummvm
        {
            enum class OplEmulator
            {
                AUTO = 0,
                MAME = 1,
                DOS_BOX = 2,
                NUKED = 4,
                WOODY = 8
            };

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
                 * OPL type to emulate.
                 * TODO remove
                 */
                enum class OplType
                {
                    OPL2,
                    DUAL_OPL2,
                    OPL3
                };

                /**
                 * Creates the specific driver with a specific type setup.
                 */
                static std::shared_ptr<OPL> create(OplEmulator oplEmulator, OplType type, const std::shared_ptr<audio::scummvm::Mixer>& mixer);
            };

        }
    }
}
