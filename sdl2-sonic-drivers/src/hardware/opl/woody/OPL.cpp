#include <hardware/opl/woody/OPL.hpp>

namespace hardware::opl::woody
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
