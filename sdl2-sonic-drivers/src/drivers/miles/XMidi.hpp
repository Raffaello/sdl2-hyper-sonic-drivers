#pragma once

#include <cstdint>
#include <string>

namespace drivers
{
    namespace miles
    {
        /*
         * XMIDI Driver Interface
         * ported from XMIDI.ASM (YAMAHA.INC)
         * This should be an abstract class
         * and the driver ADLIB.ADV (YAMAHA.INC) should be the
         * implementation with specific paramters.
         */

        constexpr const int NUM_CHAN = 16; // of MIDI channels
        constexpr const int DRIVER_MAGIC_SIZE = 43;
        constexpr const char DRIVER_MAGIC[DRIVER_MAGIC_SIZE + 1] = "Copyright (C) 1991,1992 Miles Design, Inc.\x1a";

        typedef struct driver_header_t
        {
            uint16_t driver_index_offset;   // start after magic the driver_index_t
            char magic[DRIVER_MAGIC_SIZE];
        } driver_header_t;

        typedef struct driver_index_t
        {
            uint16_t id;     // eg describe_driver = 100 etc...
            uint16_t offset; // pointing to the assebler routine, to call. useless
        } driver_index_t;

        enum class eDriverFunction {
            AIL_DESC_DRVR = 100,
            AIL_DET_DEV = 101,
            AIL_INIT_DRVR = 102,
            AIL_SERVE_DRVR = 103,
            AIL_SHUTDOWN_DRVR = 104,
            
            AIL_INDEX_VOC_BLK = 120,
            AIL_REG_SND_BUFF = 121,
            AIL_SND_BUFF_STAT = 122,
            AIL_P_VOC_FILE = 123,
            AIL_VOC_PB_STAT = 124,
            AIL_START_D_PB = 125,
            AIL_STOP_D_PB = 126,
            AIL_PAUSE_D_PB = 127,
            AIL_RESUME_D_PB = 128,
            AIL_SET_D_PB_VOL = 129,
            AIL_D_PB_VOL = 130,
            AIL_SET_D_PB_PAN = 131,
            AIL_D_PB_PAN = 132,
            AIL_F_VOC_FILE = 133,
            AIL_F_SND_BUFF = 134,
            
            AIL_STATE_TAB_SIZE = 150,
            AIL_REG_SEQ = 151,
            AIL_REL_SEQ_HND = 152,
            AIL_T_CACHE_SIZE = 153,
            AIL_DEFINE_T_CACHE = 154,
            AIL_T_REQ = 155,
            AIL_INSTALL_T = 156,
            AIL_PROTECT_T = 157,
            AIL_UNPROTECT_T = 158,

            AIL_T_STATUS = 159,
            AIL_START_SEQ = 170,
            AIL_STOP_SEQ = 171,
            AIL_RESUME_SEQ = 173,
            AIL_SEQ_STAT = 174,
            AIL_REL_VOL = 175,
            AIL_REL_TEMPO = 176,
            AIL_SET_REL_VOL = 177,
            AIL_SET_REL_TEMPO = 178,
            AIL_BEAT_CNT = 179,
            AIL_BAR_CNT = 180,
            AIL_BRA_INDEX = 181,
            
            AIL_CON_VAL = 182,
            AIL_SET_CON_VAL = 183,
            AIL_CHAN_NOTES = 185,
            AIL_SEND_CV_MSG = 186,
            AIL_SEND_SYSEX_MSG = 187,
            AIL_WRITE_DISP = 188,
            AIL_INSTALL_CB = 189,
            AIL_CANCEL_CB = 190,
            
            AIL_LOCK_CHAN = 191,
            AIL_MAP_SEQ_CHAN = 192,
            AIL_RELEASE_CHAN = 193,
            AIL_TRUE_SEQ_CHAN = 194
        };

        constexpr const int NUM_DRIVER_FUNCTIONS = 38;

        typedef struct driver_descriptor_table_t
        {
            int16_t min_api_version;
            int16_t driver_type;
            char    data_suffix[4]; // OPL2 = 'AD', OPL3='OPL' terminated by zero
            int16_t offset_devname_o; // offset to a string
            int16_t offset_devname_s;
            int16_t default_io;
            int16_t default_irq;
            int16_t default_dma;
            int16_t default_drq;
            int16_t service_rate;
            int16_t display_size;
        } driver_descriptor_table_t;

        class XMidi
        {
        public:
            // test 
            // TODO (move to a AILFile in files to actually read the drivers, not need to be completed)
            static void readDriver(const std::string& filename);
        };
    }
}