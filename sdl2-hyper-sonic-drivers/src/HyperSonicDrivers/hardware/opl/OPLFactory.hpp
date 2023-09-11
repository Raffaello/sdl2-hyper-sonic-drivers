#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <HyperSonicDrivers/audio/IMixer.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL.hpp>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>

namespace HyperSonicDrivers::hardware::opl
{
    /**
     * @defgroup audio_fmopl OPL emulation
     * @ingroup audio
     *
     * @brief OPL factory method to return an OPL emulator instance.
     * @{
     */
    class OPLFactory
    {
    public:
        OPLFactory(OPLFactory&) = delete;
        OPLFactory(OPLFactory&&) = delete;
        OPLFactory& operator=(OPLFactory&) = delete;

        /**
         * Creates the specific driver with a specific type setup.
         */
        static std::shared_ptr<OPL> create(
            const OplEmulator oplEmulator,
            const OplType type,
            const std::shared_ptr<audio::IMixer>& mixer
        );
    };
}
