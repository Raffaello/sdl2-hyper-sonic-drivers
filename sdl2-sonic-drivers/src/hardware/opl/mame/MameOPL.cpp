#include <hardware/opl/mame/MameOPL.hpp>
#include <spdlog/spdlog.h>

namespace hardware
{
    namespace opl
    {
        namespace mame
        {
            using namespace opl3;

            constexpr int OPL2_INTERNAL_FREQ = 3600000;   // The OPL2 operates at 3.6MHz;
            constexpr int OPL3_INTERNAL_FREQ = 14400000;  // The OPL3 operates at 14.4MHz;


            MameOPL::MameOPL(const std::shared_ptr<audio::scummvm::Mixer>& mixer) : EmulatedOPL(mixer),
                _chip(nullptr), _opl(nullptr)
            {}

            MameOPL::~MameOPL()
            {
                if (_chip != nullptr)
                {
                    stopCallbacks();
                    //delete _opl;
                    ymf262_shutdown(_chip);
                }

                if (_opl != nullptr)
                {
                    stopCallbacks();
                    delete _opl;
                }
            }
            bool MameOPL::init()
            {
                _init = _opl != nullptr;
                if (_init) {
                    return true;
                }

                _opl = new ymfm::ymf262(_ymfm);

                //auto rate = _opl->sample_rate(OPL3_INTERNAL_FREQ);
                //_opl->sample_rate(_mixer->getOutputRate());

                _chip = ymf262_init(0, OPL3_INTERNAL_FREQ, _mixer->getOutputRate());
                _init = _opl != nullptr;
                
                return _init;
            }
            void MameOPL::reset()
            {
                _opl->reset();
                ymf262_reset_chip(_chip);
            }
            void MameOPL::write(int a, int v)
            {
                // ???
                //_opl->write_address(a);
                //_opl->write_data(v);
                _opl->write(a, v);

                ymf262_write(_chip, a, v);
            }
            uint8_t MameOPL::read(int a)
            {
                return _opl->read(a);

                //return ymf262_read(_chip, a);
            }

            void MameOPL::writeReg(int r, int v)
            {
                _opl->write(0, r);
                _opl->write(1, v);

                ymf262_write(_chip, 0, r);
                ymf262_write(_chip, 1, v);
            }

            bool MameOPL::isStereo() const
            {
                return true;
            }

            void MameOPL::generateSamples(int16_t* buffer, int length)
            {
                constexpr int MAX_SIZE = 512;
                ymfm::ymf262::output_data b[MAX_SIZE];
                int remaining = length * 2 / _opl->OUTPUTS;
                
                int16_t buf[4][512];
                int16_t* buffers[4] = { buf[0], buf[1], buf[2], buf[3] };

                std::memset(buffer, 0, length * sizeof(int16_t));
                while (remaining > 0)
                {
                    const int numSamples = std::min(remaining, MAX_SIZE);
                    // or using _opl object or the c api yaml262_* functions.
                    _opl->generate(b, numSamples);

                    ymf262_update_one(_chip, buffers, numSamples);
                    //Interleave the samples before mixing
                    for (int i = 0; i < numSamples; i++) {
                        //buffer[i * 2] = buffers[0][i];
                        //buffer[i * 2 + 1] = buffers[1][i];
                        //buffer[i * 4 + 2] = buffers[2][i];
                        //buffer[i * 4 + 3] = buffers[3][i];


                        buffer[i + 0] = b[i].data[0];
                        buffer[i * 2 + 1] = b[i].data[1];
                        //buffer[i * 2 + 2] = b[i].data[2] * 128;
                        //buffer[i * 2 + 3] = b[i].data[3] * 128;

                        //auto a = buf[0][i] - b[i].data[0];
                        //auto z = buf[1][i] - b[i].data[1];
                        //auto c = buf[2][i] - b[i].data[2];
                        //auto d = buf[3][i] - b[i].data[3];
                        //int e = 0;
                    }
                    
                    
                    //for (int i = 0; i < numSamples; i++) {
                    //    //b[i].roundtrip_fp();
                    //    /*for (int j = 0; j < _opl->OUTPUTS/2; j++) {
                    //        auto a = b[i].data[j];
                    //        buffer[i + j] = b[i].data[j];
                    //    }*/

                    //    buffer[i*2] = b[i].data[0];
                    //    buffer[i*2 + 1] = b[i].data[1];
                    //    auto a = b[i];
                    //    if (a.data[0] != 0 ||
                    //        a.data[1] != 0 ||
                    //        a.data[2] != 0 ||
                    //        a.data[3] != 0)
                    //    {
                    //        //spdlog::info("MameOPL: b[{}]= [ {}, {}, {}, {} ]", i, a.data[0], a.data[1], a.data[2], a.data[3]);
                    //    }
                    //}

                    remaining -= numSamples;
                }
            }
        }
    }
}
