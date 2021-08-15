#include "OPL.hpp"
#include "..\OPL.hpp"

namespace hardware
{
    namespace opl
    {
        namespace woody
        {
            OPL::OPL(const ChipType chip) noexcept
                : _chip(chip)
            {
            }

            ChipType OPL::getChipType() const noexcept
            {
                return _chip;
            }
        }
    }
}
