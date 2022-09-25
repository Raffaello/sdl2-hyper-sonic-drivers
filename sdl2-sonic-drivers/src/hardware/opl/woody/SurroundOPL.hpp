/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2010 Simon Peter, <dn.tlp@gmx.net>, et al.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * surroundopl.h - Wrapper class to provide a surround/harmonic effect
 *   for another OPL emulator, by Adam Nielsen <malvineous@shikadi.net>
 *
 * Stereo harmonic algorithm by Adam Nielsen <malvineous@shikadi.net>
 * Please give credit if you use this algorithm elsewhere :-)
 */

#pragma once

#include <hardware/opl/woody/OPL.hpp>
#include <cstdint> // for uintxx_t

namespace hardware::opl::woody
{
    class SurroundOPL : public OPL
    {
    
    public:
        SurroundOPL(const int rate) noexcept;
        virtual ~SurroundOPL();

        void update(short* buf, int samples) override;
        void write(uint32_t reg, uint8_t val) override;
        uint8_t read(const uint32_t port) noexcept override;
        void writeReg(const uint16_t r, const uint16_t v) noexcept override;
        virtual void init() override;
    private:
        uint16_t _bufsize;
        int16_t* _lbuf;
        int16_t* _rbuf;
        OPL* a;
        OPL* b;
        
        uint8_t iFMReg[2][256];
        uint8_t iTweakedFMReg[2][256];
        uint8_t iCurrentTweakedBlock[2][9]; // Current value of the Block in the tweaked OPL chip
        uint8_t iCurrentFNum[2][9];         // Current value of the FNum in the tweaked OPL chip
        int8_t currChip = 0;
    };
}
