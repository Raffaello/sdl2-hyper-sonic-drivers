#pragma once

#include <cstdint>
#include <string>

namespace drivers
{
    namespace miles
    {
        /*
         * XMIDI Driver Interface
         * ported from XMIDI.ASM
         */


        constexpr const int NUM_CHAN = 16; // of MIDI channels
        constexpr const int DRIVER_MAGIC_SIZE = 43;
        constexpr const char DRIVER_MAGIC[DRIVER_MAGIC_SIZE + 1] = "Copyright (C) 1991,1992 Miles Design, Inc.\x1a";

        typedef struct driver_header_t
        {
            uint16_t index_offset;   // start after magic the driver_index_t
            char magic[DRIVER_MAGIC_SIZE];
        } driver_header_t;

        typedef struct driver_function_t
        {
            uint16_t id;     // eg describe_driver = 100 etc...
            uint16_t offset; // need to add +2 [ +1 => -1 of the extra '\0' in header magic + 2]
        };

        enum class eDriverFunction {
            DESCRIBE_DRIVER = 100,
            DETECT_DEVICE = 101,
            INIT_DRIVER = 102
            // ...
        };

        constexpr const int NUM_DRIVER_FUNCTIONS = 38;

        typedef struct driver_index_offsets_t
        {
            uint16_t describe_driver;
            uint16_t detect_device;
            uint16_t init_driver;
            uint16_t serve_driver;
            uint16_t shutdown_driver;

            uint16_t get_state_size;
            uint16_t install_callback;
            uint16_t cancel_callback;
            uint16_t register_seq;
            uint16_t release_seq;

            uint16_t start_seq;
            uint16_t stop_seq;
            uint16_t resume_seq;
            uint16_t get_seq_status;
            uint16_t get_rel_volume;
            uint16_t set_rel_volume;
            uint16_t get_rel_tempo;
            uint16_t set_rel_tempo;
            uint16_t get_control_var;
            uint16_t set_control_var;
            uint16_t get_chan_note;
            uint16_t map_seq_channel;
            uint16_t true_seq_channel;
            uint16_t get_beat_count;
            uint16_t get_bar_count;
            uint16_t branch_index;

            uint16_t send_cv_msg;
            uint16_t send_sysex_msg;
            uint16_t write_display;

            uint16_t lock_channel;
            uint16_t release_channel;

            uint16_t get_cache_size;
            uint16_t define_cache;
            uint16_t get_request;
            uint16_t install_timbre;
            uint16_t protect_timbre;
            uint16_t unprotect_timbre;
            uint16_t timbre_status;
            int16_t  minus_one;       // hardoode 0xFF ?
        } driver_index_offsets_t;
        
        class XMidi
        {
        public:
            // test
            static void readDriver(const std::string& filename);
        };
    }
}