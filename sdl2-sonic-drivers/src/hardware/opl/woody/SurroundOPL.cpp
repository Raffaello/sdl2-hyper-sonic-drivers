#include <hardware/opl/woody/SurroundOPL.hpp>
#include <hardware/opl/woody/WoodyEmuOPL.hpp>

#include <cmath> // for pow()
#include <cstring> // for memset()


namespace hardware
{
    namespace opl
    {
        using scummvm::Config;

        namespace woody
        {
            SurroundOPL::SurroundOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer)
                : EmulatedOPL(mixer),
                _type(Config::OplType::DUAL_OPL2),
                bufsize(4096)
            {
                a = new WoodyEmuOPL(mixer, false);
                b = new WoodyEmuOPL(mixer, false);
                //currType = TYPE_OPL2;
                this->lbuf = new short[this->bufsize];
                this->rbuf = new short[this->bufsize];

                memset(iFMReg, 0, sizeof(iFMReg));
                memset(iTweakedFMReg, 0, sizeof(iTweakedFMReg));
                memset(iCurrentTweakedBlock, 0, sizeof(iCurrentTweakedBlock));
                memset(iCurrentFNum, 0, sizeof(iCurrentFNum));

                init();
            }

            SurroundOPL::~SurroundOPL()
            {
                delete[] this->rbuf;
                delete[] this->lbuf;
                delete a;
                delete b;
            }

            uint8_t SurroundOPL::read(int a)
            {
                return uint8_t();
            }

            bool SurroundOPL::init()
            {
                bool res;

                res = a->init();
                res &= b->init();
                for (int c = 0; c < 2; c++)
                {
                    for (int i = 0; i < 256; i++)
                    {
                        this->iFMReg[c][i] = 0;
                        this->iTweakedFMReg[c][i] = 0;
                    }
                    for (int i = 0; i < 9; i++)
                    {
                        this->iCurrentTweakedBlock[c][i] = 0;
                        this->iCurrentFNum[c][i] = 0;
                    }
                }

                return res;
            }

            void SurroundOPL::reset()
            {
            }

            void SurroundOPL::write(int a, int v)
            {
                // TODO not sure is correct
                writeReg(a, v);
            }

            void SurroundOPL::writeReg(int r, int v)
            {
                a->writeReg(r, v);

                // Transpose the other channel to produce the harmonic effect
                int iChannel = -1;
                int iRegister = r; // temp
                int iValue = v; // temp
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
                            b->writeReg(iAdditionalReg, iAdditionalValue);
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
                            b->writeReg(iAdditionalReg, iNewB0Value);
                            this->iTweakedFMReg[this->currChip][iAdditionalReg] = iNewB0Value;
                        } // else the note is not playing, the upper bits will be set when the note is next played

                    } // if (register 0xB0 or 0xA0)

                } // if (a register we're interested in)

                // Now write to the original register with a possibly modified value
                b->writeReg(iRegister, iValue);
                this->iTweakedFMReg[this->currChip][iRegister] = iValue;
            }

            bool SurroundOPL::isStereo() const
            {
                return true;
            }

            void SurroundOPL::generateSamples(int16_t* buffer, int numSamples)
            {
                if (numSamples * 2 > this->bufsize) {
                    // Need to realloc the buffer
                    delete[] this->rbuf;
                    delete[] this->lbuf;
                    this->bufsize = numSamples * 2;
                    this->lbuf = new short[this->bufsize];
                    this->rbuf = new short[this->bufsize];
                }

                a->readBuffer(this->lbuf, numSamples);
                b->readBuffer(this->rbuf, numSamples);

                // Copy the two mono OPL buffers into the stereo buffer
                for (int i = 0; i < numSamples; i++) {
                    if (this->_use16bit) {
                        buffer[i * 2] = this->lbuf[i];
                        buffer[i * 2 + 1] = this->rbuf[i];
                    }
                    else {
                        ((char*)buffer)[i * 2] = ((char*)this->lbuf)[i];
                        ((char*)buffer)[i * 2 + 1] = ((char*)this->rbuf)[i];
                    }
                }
            }
        }
    }
}
