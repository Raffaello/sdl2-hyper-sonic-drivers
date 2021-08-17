#pragma once

#include <cstdint>
#include <string>
#include <hardware/opl/OPL.hpp>
#include <audio/scummvm/Mixer.hpp>

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
            };

            // TODO: remove is redundant DriverId
            typedef OplEmulator DriverId;

            typedef struct
            {
                // TOOD: replace with std::string
                const char* name;
                const char* description;

                DriverId id;    // A unique ID for each driver
                uint32_t flags; // Capabilities of this driver
            } EmulatorDescription;

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
                enum class OplFlags
                {
                    NONE = 0,
                    OPL2 = (1 << 0),
                    DUAL_OPL2 = (1 << 1),
                    OPL3 = (1 << 2)
                };

                /**
                 * OPL type to emulate.
                 */
                enum class OplType
                {
                    OPL2,
                    DUAL_OPL2,
                    OPL3
                };

                /**
                 * Get a list of all available OPL emulators.
                 * @return list of all available OPL emulators, terminated by a zero entry
                 * 
                 * TODO: remove pointer in return type
                 */
                static const EmulatorDescription* getAvailable();

                /**
                 * Returns the driver id of a given name.
                 */
                //static DriverId parse(const std::string& name);

                /**
                 * @return The driver description for the given id or 0 in case it is not
                 *         available.
                 */
                static const EmulatorDescription* findDriver(DriverId id);

                /**
                 * Detects a driver for the specific type.
                 *
                 * @return Returns a valid driver id on success, -1 otherwise.
                 */
                //static DriverId detect(OplType type);

                /**
                 * Creates the specific driver with a specific type setup.
                 */
                static OPL* create(DriverId driver, OplType type, const std::shared_ptr<audio::scummvm::Mixer> mixer);

                /**
                 * Wrapper to easily init an OPL chip, without specifing an emulator.
                 * By default it will try to initialize an OPL2 emulator, thus an AdLib card.
                 */
                //static OPL* create(OplType type = OplType::OPL2);

            private:
                static const EmulatorDescription _drivers[];
            };

        }
    }
}
