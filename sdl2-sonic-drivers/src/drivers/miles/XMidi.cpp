#include "XMidi.hpp"
#include <string>
#include <SDL2/SDL_rwops.h>
#include <stdexcept>

namespace drivers
{
    namespace miles
    {
        void XMidi::readDriver(const std::string& filename)
        {
            SDL_RWops* f = SDL_RWFromFile(filename.c_str(), "rb");
            
            if (nullptr == f) {
                throw std::invalid_argument("");
            }

            driver_header_t header;
            header.index_offset = SDL_ReadLE16(f);
            SDL_RWread(f, header.magic, DRIVER_MAGIC_SIZE, 1);
            if (strncmp(DRIVER_MAGIC, header.magic, DRIVER_MAGIC_SIZE) != 0) {
                throw std::invalid_argument("");
            }

            /*if (tell != header.index_offset) {
                throw std::invalid_argument("");
            }*/

            //driver_index_offsets_t offsets;
            //// - 1 is because there is 1 extra char '\0' in the magic, +2 is the padding 0x00 0x00
            //offsets.describe_driver = SDL_ReadLE16(f);
            //offsets.detect_device = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.init_driver = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.serve_driver = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.shutdown_driver = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //
            //offsets.get_state_size = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.install_callback = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.cancel_callback = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.register_seq = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.release_seq = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //
            //offsets.start_seq = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.stop_seq = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.resume_seq = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_seq_status = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_rel_volume = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.set_rel_volume = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_rel_tempo = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.set_rel_tempo = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_control_var = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.set_control_var = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_chan_note = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.map_seq_channel = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.true_seq_channel = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_beat_count = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_bar_count = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.branch_index = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //
            //offsets.send_cv_msg = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.send_sysex_msg = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.write_display = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //
            //offsets.lock_channel = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.release_channel = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //
            //offsets.get_cache_size = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.define_cache = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.get_request = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.install_timbre = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.protect_timbre = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.unprotect_timbre = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.timbre_status = SDL_ReadLE16(f) + sizeof(driver_header_t) + 1;
            //offsets.minus_one = SDL_ReadLE16(f);
            //
            //if (offsets.minus_one != -1) {
            //    throw std::invalid_argument("");
            //}

            driver_function_t funcs[NUM_DRIVER_FUNCTIONS];
            SDL_RWread(f, funcs, sizeof(driver_function_t), NUM_DRIVER_FUNCTIONS);
            int16_t minusOne = SDL_ReadLE16(f);

            int64_t tell = SDL_RWtell(f);
            int zero = SDL_ReadLE16(f);
            /*if (zero != 0) {
                throw std::invalid_argument("");
            }*/

             tell = SDL_RWtell(f);
             
            SDL_RWseek(f, funcs[0].offset, RW_SEEK_SET);
            constexpr char ADLIB[33] = "Ad Lib(R) Music Synthesizer Card";
            char desc[33];
            SDL_RWread(f, desc, 33, 1);
            if (strncmp(ADLIB, desc, 33) != 0) {
                int i = 0;
            }
            
            SDL_RWclose(f);
        }
    }
}