#pragma once

#include <cstdint>

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
            OPL2,
            OPL2_DUAL,
            OPL3,
            OPL2_SURROUND_OPL, /// OPL2 emulation from surroundopl code, using 2 wemu istances
            //OPL3_WEMU,         /// OPL3 emulation from wemuopl code (using OPL_3)
            //OPL_woodyopl,      /// the only one containing hardware emulation to retro eng.
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
        class OPL
        {
        public:
            ChipType getChipType() const noexcept;
            virtual void write(int reg, int val) = 0; // combined register select + data write
            //virtual void setchip(int n) // select OPL chip
            virtual void init(void) = 0; // reinitialize OPL chip(s)
        protected:
            ChipType _chip;
            // TODO: review this variable as it is almost useless?
            int8_t   _currentChip;       // currently selected OPL chip number (0 or 1 actually)
        };
    }
}
