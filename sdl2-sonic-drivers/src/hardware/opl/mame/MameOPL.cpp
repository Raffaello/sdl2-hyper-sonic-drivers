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


            MameOPL::MameOPL(const std::shared_ptr<audio::scummvm::Mixer> mixer) : EmulatedOPL(mixer),
                _opl(nullptr)
            {}

            MameOPL::~MameOPL()
            {
                if (_opl != nullptr)
                {
                    stopCallbacks();
                    //delete _opl;
                    ymf262_shutdown(_opl);
                }
            }
            bool MameOPL::init()
            {
                if (_opl != nullptr) {
                    return true;
                }

                //_opl = new ymfm::ymf262(_ymfm);
                //_opl->sample_rate(_mixer->getOutputRate());
                //_opl->reset();

                _opl = ymf262_init(0, OPL3_INTERNAL_FREQ, _mixer->getOutputRate());

                return _opl != nullptr;
            }
            void MameOPL::reset()
            {
                //_opl->reset();
                ymf262_reset_chip(_opl);
            }
            void MameOPL::write(int a, int v)
            {
                // ???
                //_opl->write_address(a);
                //_opl->write_data(v);
                //_opl->write(a, v);

                ymf262_write(_opl, a, v);
            }
            uint8_t MameOPL::read(int a)
            {
                //return _opl->read(a);

                return ymf262_read(_opl, a);
            }

            void MameOPL::writeReg(int r, int v)
            {
                //_opl->write(r, v);
                ymf262_write(_opl, 0, r);
                ymf262_write(_opl, 1, v);
            }

            bool MameOPL::isStereo() const
            {
                return true;
            }

            void MameOPL::generateSamples(int16_t* buffer, int length)
            {
                constexpr int MAX_SIZE = 512;
                //ymfm::ymf262::output_data b[MAX_SIZE];
                //b = new ymfm::ymf262::output_data[1024];
                //int remaining = length * 2 / _opl->OUTPUTS;
                
                int remaining = length / 2;
                int16_t buf[4][1024];
                //int16_t result[1024][2];
                int16_t* buffers[4] = { buf[0], buf[1], buf[2], buf[3] };

                while (remaining > 0)
                {
                    const int numSamples = std::min(remaining, MAX_SIZE);

                    //_opl->generate(b, numSamples);

                    ymf262_update_one(_opl, buffers, numSamples);
                    //Interleave the samples before mixing
                    for (int i = 0; i < numSamples; i++) {
                        //result[i][0] = buf[0][i];
                        //result[i][1] = buf[1][i];
                        buffer[i * 2] = buf[0][i];
                        buffer[i * 2 + 1] = buf[2][i];
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
