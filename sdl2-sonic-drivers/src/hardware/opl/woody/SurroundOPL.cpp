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
 * surroundopl.cpp - Wrapper class to provide a surround/harmonic effect
 *   for another OPL emulator, by Adam Nielsen <malvineous@shikadi.net>
 *
 * Stereo harmonic algorithm by Adam Nielsen <malvineous@shikadi.net>
 * Please give credit if you use this algorithm elsewhere :-)
 */

#include <cmath> // for pow()
#include <cstring> // for memset()
#include <hardware/opl/woody/SurroundOPL.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>

namespace hardware::opl::woody
{
    // The right-channel is increased in frequency by itself divided by this amount.
    // The right value should not noticeably change the pitch, but it should provide
    // a nice stereo harmonic effect.
    constexpr double FREQ_OFFSET = 128.0;

    // Number of FNums away from the upper/lower limit before switching to the next
    // block (octave.)  By rights it should be zero, but for some reason this seems
    // to cut it too close and the transposed OPL doesn't hit the right note all the
    // time.  Setting it higher means it will switch blocks sooner and that seems
    // to help.  Don't set it too high or it'll get stuck in an infinite loop if
    // one block is too high and the adjacent block is too low ;-)
    constexpr int NEWBLOCK_LIMIT = 32;

    SurroundOPL::SurroundOPL(const int rate) noexcept
        : OPL(), bufsize(2048)
    {
        a = std::make_unique<WoodyEmuOPL>(rate);
        b = std::make_unique<WoodyEmuOPL>(rate);
        allocateBuffers();

        memset(iFMReg, 0, sizeof(iFMReg));
        memset(iTweakedFMReg, 0, sizeof(iTweakedFMReg));
        memset(iCurrentTweakedBlock, 0, sizeof(iCurrentTweakedBlock));
        memset(iCurrentFNum, 0, sizeof(iCurrentFNum));

        init();
    }

    void SurroundOPL::update(short* buf, int samples)
    {
        if (samples > this->bufsize) {
            // Need to realloc the buffer
            this->bufsize = samples * 2;
            allocateBuffers();
        }

        a->update(this->lbuf.get(), samples);
        b->update(this->rbuf.get(), samples);

        // Copy the two mono OPL buffers into the stereo buffer
        for (int i = 0; i < samples; i++)
        {
            buf[i * 2] = this->lbuf.get()[i];
            buf[i * 2 + 1] = this->rbuf.get()[i];
        }
    }

    void SurroundOPL::write(uint32_t reg, uint8_t val)
    {
        a->write(reg, val);

        // Transpose the other channel to produce the harmonic effect
        int iChannel = -1;
        int iRegister = reg; // temp
        uint8_t iValue = val; // temp
        if ((iRegister >> 4 == 0xA) || (iRegister >> 4 == 0xB)) iChannel = iRegister & 0x0F;

        // Remember the FM state, so that the harmonic effect can access
        // previously assigned register values.
        this->iFMReg[this->currChip][iRegister] = iValue;

        if ((iChannel >= 0)) {// && (i == 1)) {
            uint8_t iBlock = (this->iFMReg[this->currChip][0xB0 + iChannel] >> 2) & 0x07;
            uint16_t iFNum = ((this->iFMReg[this->currChip][0xB0 + iChannel] & 0x03) << 8) | this->iFMReg[this->currChip][0xA0 + iChannel];
            //double dbOriginalFreq = 50000.0 * (double)iFNum * pow(2, iBlock - 20);
            double dbOriginalFreq = 49716.0 * (double)iFNum * pow(2, iBlock - 20);

            uint8_t iNewBlock = iBlock;
            uint16_t iNewFNum;

            // Adjust the frequency and calculate the new FNum
            //double dbNewFNum = (dbOriginalFreq+(dbOriginalFreq/FREQ_OFFSET)) / (50000.0 * pow(2, iNewBlock - 20));
            //#define calcFNum() ((dbOriginalFreq+(dbOriginalFreq/FREQ_OFFSET)) / (50000.0 * pow(2, iNewBlock - 20)))
#define calcFNum() ((dbOriginalFreq+(dbOriginalFreq/FREQ_OFFSET)) / (49716.0 * pow(2, iNewBlock - 20)))
            double dbNewFNum = calcFNum();

            // Make sure it's in range for the OPL chip
            if (dbNewFNum > 1023 - NEWBLOCK_LIMIT) {
                // It's too high, so move up one block (octave) and recalculate

                if (iNewBlock > 6) {
                    // Uh oh, we're already at the highest octave!
    //              AdPlug_LogWrite("OPL WARN: FNum %d/B#%d would need block 8+ after being transposed (new FNum is %d)\n",
    //                  iFNum, iBlock, (int)dbNewFNum);
                    // The best we can do here is to just play the same note out of the second OPL, so at least it shouldn't
                    // sound *too* bad (hopefully it will just miss out on the nice harmonic.)
                    iNewBlock = iBlock;
                    iNewFNum = iFNum;
                }
                else {
                    iNewBlock++;
                    iNewFNum = (uint16_t)calcFNum();
                }
            }
            else if (dbNewFNum < 0 + NEWBLOCK_LIMIT) {
                // It's too low, so move down one block (octave) and recalculate

                if (iNewBlock == 0) {
                    // Uh oh, we're already at the lowest octave!
    //              AdPlug_LogWrite("OPL WARN: FNum %d/B#%d would need block -1 after being transposed (new FNum is %d)!\n",
    //                  iFNum, iBlock, (int)dbNewFNum);
                    // The best we can do here is to just play the same note out of the second OPL, so at least it shouldn't
                    // sound *too* bad (hopefully it will just miss out on the nice harmonic.)
                    iNewBlock = iBlock;
                    iNewFNum = iFNum;
                }
                else {
                    iNewBlock--;
                    iNewFNum = (uint16_t)calcFNum();
                }
            }
            else {
                // Original calculation is within range, use that
                iNewFNum = (uint16_t)dbNewFNum;
            }

            // Sanity check
            if (iNewFNum > 1023) {
                // Uh oh, the new FNum is still out of range! (This shouldn't happen)
    //          AdPlug_LogWrite("OPL ERR: Original note (FNum %d/B#%d is still out of range after change to FNum %d/B#%d!\n",
    //              iFNum, iBlock, iNewFNum, iNewBlock);
                // The best we can do here is to just play the same note out of the second OPL, so at least it shouldn't
                // sound *too* bad (hopefully it will just miss out on the nice harmonic.)
                iNewBlock = iBlock;
                iNewFNum = iFNum;
            }

            if ((iRegister >= 0xB0) && (iRegister <= 0xB8)) {

                // Overwrite the supplied value with the new F-Number and Block.
                iValue = (iValue & ~0x1F) | (iNewBlock << 2) | ((iNewFNum >> 8) & 0x03);

                this->iCurrentTweakedBlock[this->currChip][iChannel] = iNewBlock; // save it so we don't have to update register 0xB0 later on
                this->iCurrentFNum[this->currChip][iChannel] = iNewFNum;

                if (this->iTweakedFMReg[this->currChip][0xA0 + iChannel] != (iNewFNum & 0xFF)) {
                    // Need to write out low bits
                    uint8_t iAdditionalReg = 0xA0 + iChannel;
                    uint8_t iAdditionalValue = iNewFNum & 0xFF;
                    b->write(iAdditionalReg, iAdditionalValue);
                    this->iTweakedFMReg[this->currChip][iAdditionalReg] = iAdditionalValue;
                }
            }
            else if ((iRegister >= 0xA0) && (iRegister <= 0xA8)) {

                // Overwrite the supplied value with the new F-Number.
                iValue = iNewFNum & 0xFF;

                // See if we need to update the block number, which is stored in a different register
                uint8_t iNewB0Value = (this->iFMReg[this->currChip][0xB0 + iChannel] & ~0x1F) | (iNewBlock << 2) | ((iNewFNum >> 8) & 0x03);
                if (
                    (iNewB0Value & 0x20) && // but only update if there's a note currently playing (otherwise we can just wait
                    (this->iTweakedFMReg[this->currChip][0xB0 + iChannel] != iNewB0Value)   // until the next noteon and update it then)
                    ) {
                    //              AdPlug_LogWrite("OPL INFO: CH%d - FNum %d/B#%d -> FNum %d/B#%d == keyon register update!\n",
                    //                  iChannel, iFNum, iBlock, iNewFNum, iNewBlock);
                                        // The note is already playing, so we need to adjust the upper bits too
                    uint8_t iAdditionalReg = 0xB0 + iChannel;
                    b->write(iAdditionalReg, iNewB0Value);
                    this->iTweakedFMReg[this->currChip][iAdditionalReg] = iNewB0Value;
                } // else the note is not playing, the upper bits will be set when the note is next played

            } // if (register 0xB0 or 0xA0)

        } // if (a register we're interested in)

        // Now write to the original register with a possibly modified value
        b->write(iRegister, iValue);
        this->iTweakedFMReg[this->currChip][iRegister] = iValue;
    }

    uint8_t SurroundOPL::read(const uint32_t port) noexcept
    {
        return a->read(port);
    }

    void SurroundOPL::writeReg(const uint16_t r, const uint16_t v) noexcept
    {
        a->writeReg(r, v);
        b->writeReg(r, v);
    }

    int32_t SurroundOPL::getSampleRate() const noexcept
    {
        return a->getSampleRate();
    }

    void SurroundOPL::init()
    {
        a->init();
        b->init();
        for (int c = 0; c < 2; c++) {
            for (int i = 0; i < 256; i++) {
                this->iFMReg[c][i] = 0;
                this->iTweakedFMReg[c][i] = 0;
            }
            for (int i = 0; i < 9; i++) {
                this->iCurrentTweakedBlock[c][i] = 0;
                this->iCurrentFNum[c][i] = 0;
            }
        }
    }

    void SurroundOPL::allocateBuffers()
    {
        this->lbuf = std::make_unique<int16_t>(this->bufsize);
        this->rbuf = std::make_unique<int16_t>(this->bufsize);
    }
}
