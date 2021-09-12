#pragma once

#include <files/File.hpp>
#include <cstdint>
#include <string>

namespace files
{
    /// <summary>
    /// AIL XMIDI Driver Interface File parsing
    /// ported from XMIDI.ASM (YAMAHA.INC)
    /// </summary>
    /// TODO: this is working for only YAMAHAINC file.
    /// proably should be specialized.
    /// BODY: This should be an abstract class
    /// and the driver ADLIB.ADV(YAMAHA.INC) should be the
    /// implementation with specific paramters.
    class AILDriverFile : public File
    {
    public:
        /*
        IFDEF YM3812
            NUM_VOICES      equ 9; # of physical voices available
            NUM_SLOTS       equ 16; # of virtual voices available
            ELSEIFDEF YMF262
            NUM_VOICES      equ 18; # of physical voices available
            NUM_4OP_VOICES  equ 6
            NUM_SLOTS       equ 20; # of virtual voices available
        ENDIF
        */
        static const int DRIVER_MAGIC_SIZE = 43;
        static const int NUM_DRIVER_FUNCTIONS = 38;
        static const int FREQ_TABLE_SIZE = 192;
        static const int NOTE_TABLE_SIZE = 12 * 8;
        static const int ARRAY_INIT_SIZE = 245;
        static const int NUM_CHAN = 16; // of MIDI channels
        static const int PAN_GRAPH_SIZE = NUM_CHAN * 8;
        static const int NUM_VOICES = 18; // OPL3 (OPL2 is half)
        static const int NUM_4OP_VOICES = 6;

        typedef struct driver_header_t
        {
            uint16_t driver_index_offset;   // start after magic the driver_index_t
            char magic[DRIVER_MAGIC_SIZE];
        } driver_header_t;

        typedef struct driver_index_t
        {
            uint16_t id;     // eg describe_driver = 100 etc...
            uint16_t offset; // pointing to the assembler routine to call.
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

        // used for driver_descriptor_table_t.driver_type
        enum class eDriverType
        {
            XMIDI_EMULATION = 3
        };

        typedef struct driver_descriptor_table_t
        {
            int16_t min_api_version;
            int16_t driver_type;
            // Global Timbre file extension
            // OPL2 = 'AD\0\0', OPL3='OPL\0' backward compatible with 'AD'.
            char    data_suffix[4]; 
            int16_t offset_devname_o; // offset to a string
            int16_t offset_devname_s;
            int16_t default_io;
            int16_t default_irq;
            int16_t default_dma;
            int16_t default_drq;
            int16_t service_rate;
            int16_t display_size;
        } driver_descriptor_table_t;


        typedef struct BNK_style_timbre_definition_t
        {
            uint16_t length;
            uint8_t  transpose;
            uint8_t  mod_AVEKM; // op_0 = FM modulator
            uint8_t  mod_KSLTL;
            uint8_t  mod_AD;
            uint8_t  mod_SR;
            uint8_t  mod_WS;
            uint8_t  fb_c;
            uint8_t  car_AVEKM; // op_1 = FM carrier
            uint8_t  car_KSLTL;
            uint8_t  car_AD;
            uint8_t  car_SR;
            uint8_t  car_WS;
        } BNK_style_timbre_definition_t;

        typedef struct OPL3_BNK_style_timbre_definition_t
        {
            BNK_style_timbre_definition_t opl2;
            uint8_t O_mod_AVEKM; // op_2
            uint8_t O_mod_KSLTL;
            uint8_t O_mod_AD;
            uint8_t O_mod_SR;
            uint8_t O_mod_WS;
            uint8_t O_fb_c;
            uint8_t O_car_AVEKM; // op_3
            uint8_t O_car_KSLTL;
            uint8_t O_car_AD;
            uint8_t O_car_SR;
            uint8_t O_car_WS;
        } OPL3_BNK_style_timbre_defintion_t;

        AILDriverFile(const std::string& filename);
        virtual ~AILDriverFile();

        const int16_t getMinimumApiVersion() const noexcept;
        const int16_t getDriverType() const noexcept;
        const char*   getDataSuffix() const noexcept;
        const std::string getDeviceName_o() const noexcept;
        const std::string getDeviceName_s() const noexcept;
        //const int16_t getDefault_io() const noexcept;
        
        const int16_t getServiceRate() const noexcept;

    private:
        driver_header_t _header;
        driver_index_t _funcs[NUM_DRIVER_FUNCTIONS];
        driver_descriptor_table_t _ddt;
        std::string _deviceName_o;
        std::string _deviceName_s;
        bool _isStereo; // control flag

        uint16_t _freq_table[FREQ_TABLE_SIZE];
        uint8_t _note_octave[NOTE_TABLE_SIZE];
        uint8_t _note_halftone[NOTE_TABLE_SIZE];
        uint8_t _array0_init[ARRAY_INIT_SIZE];
        uint8_t _array1_init[ARRAY_INIT_SIZE];
        uint8_t _vel_graph[NUM_CHAN];
        uint8_t _pan_graph[PAN_GRAPH_SIZE]; // only if stereo...
        uint8_t _op_0[NUM_VOICES];
        uint8_t _op_1[NUM_VOICES];
        uint8_t _op_index[NUM_VOICES * 2];
        uint8_t _op_array[NUM_VOICES * 2];
        uint8_t _voice_num[NUM_VOICES];
        uint8_t _voice_array[NUM_VOICES];
        uint8_t _op4_base[NUM_VOICES];
        uint8_t _alt_voice[NUM_VOICES];
        
        // only if YM262 (OPL3)
        uint8_t _alt_op_0[NUM_VOICES];
        uint8_t _alt_op_1[NUM_VOICES];
        uint8_t _conn_sel[NUM_VOICES];
        uint8_t _op4_voice[NUM_4OP_VOICES];
        uint8_t _carrier_01[4];
        uint8_t _carrier_23[4];
        // end only if

        int findFunctionIndex(const eDriverFunction func) const noexcept;
        int callFunction(const eDriverFunction func);
        int callFunction(const int func_index);

        void readHeader();
        void readFunctions();
        void readDriverDescriptorTable();
    };
}
