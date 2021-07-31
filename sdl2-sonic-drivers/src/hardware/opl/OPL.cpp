#include "OPL.hpp"

namespace hardware
{
    namespace opl
    {
        ChipType OPL::getChipType() const noexcept
        {
            return _chip;
        }
    }
}
