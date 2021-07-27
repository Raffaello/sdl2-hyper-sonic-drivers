#pragma once

namespace hardware
{
    /// <summary>
    /// @see https://moddingwiki.shikadi.net/wiki/OPL_chip
    /// </summary>
    namespace opl
    {
        // shouldn't done from inheritance class instead?
        enum class ChipType
        {
            OPL2 = 0,
            OPL2_DUAL,
            OPL3,
            OPL2_SURROUND_OPL /// OPL2 emulation from surroundopl code, using 2 wemu istances
            OPL3_WEMU         /// OPL3 emulation from wemuopl code
            OPL_woodyopl      /// the only one containing hardware emulation to retro eng.
            OPL_dbopl         /// dbopl file  like woodyopl but in integer instead of float.
        };

        // TODO: Config implementation
        /*enum OplEmulator {
            kAuto = 0,
            kMame = 1,
            kDOSBox = 2,
            kALSA = 3,
            kNuked = 4,
            kOPL2LPT = 5,
            kOPL3LPT = 6
        };*/
        
        //this opl class could be moved just in hardware namespace as a general interface for opl namespace
        class opl
        {
        public:
            ChipType getChipType() const noexcept;
        protected:
            ChipType _chip;
        };
    }
}
