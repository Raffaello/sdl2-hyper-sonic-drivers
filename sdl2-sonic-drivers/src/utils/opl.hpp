#pragma once

#include <hardware/opl/OPL.hpp>
#include <memory>
#include <utils/algorithms.hpp>

namespace utils
{
    // These are offsets from the base I/O address.
    constexpr int FM = 8;       // SB (mono) ports (e.g. 228H and 229H)
    constexpr int PROFM1 = 0;   // On CT-1330, this is left OPL-2.  On CT-1600 and
                                // later cards, it's OPL-3 bank 0.
    constexpr int PROFM2 = 2;   // On CT-1330, this is right OPL-2.  On CT-1600 and
                                // later cards, it's OPL-3 bank 1.

    /// <summary>
    /// This outputs a value to a specified FM register at a specified FM port.
    /// </summary>
    /// <param name="port"></param>
    /// <param name="reg"></param>
    /// <param name="val"></param>
    /// <param name="opl"></param>
    void FMoutput(unsigned port, int reg, int val, std::shared_ptr<hardware::opl::OPL> opl);
    
    /// <summary>
    /// This function outputs a value to a specified FM register at the Sound
    /// Blaster/Adlib (mono) port address.
    /// </summary>
    void fm(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl);

    /// <summary>
    /// This function outputs a value to a specified FM register at the Sound
    /// Blaster Pro left FM port address(or OPL - 3 bank 0).
    /// </summary>
    /// <param name="opl"></param>
    /// <returns></returns>
    void Profm1(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl);
    
    /// <summary>
    /// This function outputs a value to a specified FM register at the Sound
    /// Blaster Pro right FM port address(or OPL - 3 bank 1).
    /// </summary>
    /// <param name="opl"></param>
    /// <returns></returns>
    void Profm2(int reg, int val, std::shared_ptr<hardware::opl::OPL> opl);

    /// <summary>
    /// detect OPL2: not exactly working with emulators
    /// It init the opl if it wasn't already
    /// It reset the opl
    /// </summary>
    bool detectOPL2(std::shared_ptr<hardware::opl::OPL> opl);

    /// <summary>
    /// detect OPL3: not exactly working with emulators
    /// It init the opl if it wasn't already
    /// It reset the opl
    /// It calls detectOPL2
    /// </summary>
    bool detectOPL3(std::shared_ptr<hardware::opl::OPL> opl);
}
