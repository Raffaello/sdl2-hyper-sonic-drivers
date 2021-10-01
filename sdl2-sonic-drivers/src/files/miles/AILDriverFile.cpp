#include <files/miles/AILDriverFile.hpp>
#include <cstring>
#include <vector>
#include <map>

namespace files
{
    namespace miles
    {
        constexpr char DRIVER_MAGIC[AILDriverFile::DRIVER_MAGIC_SIZE + 1] = "Copyright (C) 1991,1992 Miles Design, Inc.\x1a";

        constexpr int FUNCTION_BEGIN_SIZE = 8;
        constexpr uint8_t FUNCTION_BEGIN[] =
        { 0x55, 0x8B, 0xEC, 0x1E, 0x56, 0x57, 0x9C, 0xFA };

        constexpr int FUNCTION_RETURN_SIZE = 12;
        constexpr uint8_t FUNCTION_RETURN[FUNCTION_RETURN_SIZE] =
        { 0x80, 0xCF, 0x00, 0x0E, 0xE8, 0xFA, 0xFF, 0x5F, 0x5E, 0x1F, 0x5D, 0xCB };

        const std::map<int, int> FUNCTIONS_RESULT{
            { (int)AILDriverFile::eDriverFunction::AIL_DESC_DRVR, 8 },

        };


        AILDriverFile::AILDriverFile(const std::string& filename) : File(filename)
        {
            readHeader();
            readFunctions();
            readDriverDescriptorTable();
        }

        AILDriverFile::~AILDriverFile()
        {
        }

        const int16_t AILDriverFile::getMinimumApiVersion() const noexcept
        {
            return _ddt.min_api_version;
        }

        const int16_t AILDriverFile::getDriverType() const noexcept
        {
            return _ddt.driver_type;
        }

        const char* AILDriverFile::getDataSuffix() const noexcept
        {
            return _ddt.data_suffix;
        }

        const std::string AILDriverFile::getDeviceName_o() const noexcept
        {
            return _deviceName_o;
        }

        const std::string AILDriverFile::getDeviceName_s() const noexcept
        {
            return _deviceName_s;
        }

        const int16_t AILDriverFile::getServiceRate() const noexcept
        {
            return _ddt.service_rate;
        }

        int AILDriverFile::findFunctionIndex(const eDriverFunction func) const noexcept
        {
            for (int i = 0; i < NUM_DRIVER_FUNCTIONS; i++) {
                if (_funcs[i].id == static_cast<int>(func)) {
                    return i;
                }
            }

            return -1;
        }

        int AILDriverFile::callFunction(const eDriverFunction func)
        {
            return callFunction(findFunctionIndex(func));
        }

        int AILDriverFile::callFunction(const int func_index)
        {
            if (func_index == -1 || _funcs[func_index].offset == 0) {
                throw std::runtime_error("whoops! where the issue here?");
            }

            seek(_funcs[func_index].offset, std::fstream::beg);

            uint8_t buf1[FUNCTION_BEGIN_SIZE];
            uint8_t buf2[FUNCTION_RETURN_SIZE];

            read(buf1, FUNCTION_BEGIN_SIZE);
            for (int i = 0; i < FUNCTION_BEGIN_SIZE; i++) {
                _assertValid(buf1[i] == FUNCTION_BEGIN[i]);
            }

            std::vector<uint8_t> buf;

            // TODO: can improve the while loop, but not sure is useful
            // BODY: can compute the size to read from the next closest offsets
            // BODY: need to be searched and then subtract the 2 start end string
            // BODY: constant function.
            // BODY: besides after read the asm code, is useless
            // BODY: as need a asm 8086 interpreter to understand and convert it.

            uint8_t c1;
            uint8_t c2;
            bool stop = false;
            while (!stop)
            {
                c1 = readU8();
                c2 = readU8();
                if (c1 == FUNCTION_RETURN[0] && c2 == FUNCTION_RETURN[1]) {
                    stop = true;
                    seek(-2, std::fstream::cur);
                }
                else {
                    buf.push_back(c1);
                    buf.push_back(c2);
                }
            }

            read(buf2, FUNCTION_RETURN_SIZE);
            for (int i = 0; i < FUNCTION_RETURN_SIZE; i++) {
                _assertValid(buf2[i] == FUNCTION_RETURN[i]);
            }


            return buf[FUNCTIONS_RESULT.at(_funcs[func_index].id)];
        }

        void AILDriverFile::readHeader()
        {
            _header.driver_index_offset = readLE16();
            read(_header.magic, DRIVER_MAGIC_SIZE);

            _assertValid(strncmp(DRIVER_MAGIC, _header.magic, DRIVER_MAGIC_SIZE) == 0);
            _assertValid(tell() == _header.driver_index_offset);
        }

        void AILDriverFile::readFunctions()
        {
            read(_funcs, sizeof(driver_index_t) * NUM_DRIVER_FUNCTIONS);
            int16_t minusOne = readLE16();
            _assertValid(minusOne == -1);

            // sanity check, might not be ok, could be not in 0 position.
            //_assertValid(_funcs[0].id == static_cast<int>(eDriverFunction::AIL_DESC_DRVR));
        }

        void AILDriverFile::readDriverDescriptorTable()
        {
            int res = callFunction(eDriverFunction::AIL_DESC_DRVR);
            seek(res, std::fstream::beg);

            read(&_ddt, sizeof(driver_descriptor_table_t));
            if (_ddt.offset_devname_o != 0) {
                seek(_ddt.offset_devname_o, std::fstream::beg);
                _deviceName_o = _readStringFromFile();
            }

            if (_ddt.offset_devname_s != 0) {
                seek(_ddt.offset_devname_s, std::fstream::beg);
                _deviceName_s = _readStringFromFile();
            }

            _assertValid(readU8() == 0); // 0 extra padding

            // setStereo()
            _isStereo = strncmp(_ddt.data_suffix, "OPL", 4) == 0;

            read(_freq_table, FREQ_TABLE_SIZE * sizeof(int16_t));
            read(_note_octave, NOTE_TABLE_SIZE * sizeof(int8_t));
            read(_note_halftone, NOTE_TABLE_SIZE * sizeof(int8_t));
            read(_array0_init, ARRAY_INIT_SIZE * sizeof(uint8_t));
            read(_array1_init, ARRAY_INIT_SIZE * sizeof(uint8_t));
            read(_vel_graph, NUM_CHAN * sizeof(uint8_t));

            if (_isStereo) {
                read(_pan_graph, PAN_GRAPH_SIZE * sizeof(uint8_t));
            }

            read(_op_0, NUM_VOICES * sizeof(uint8_t));
            read(_op_1, NUM_VOICES * sizeof(uint8_t));
            read(_op_index, NUM_VOICES * 2 * sizeof(uint8_t));
            read(_op_array, NUM_VOICES * 2 * sizeof(uint8_t));

            read(_voice_num, NUM_VOICES * sizeof(uint8_t));
            read(_voice_array, NUM_VOICES * sizeof(uint8_t));
            read(_op4_base, NUM_VOICES * sizeof(uint8_t));
            read(_alt_voice, NUM_VOICES * sizeof(uint8_t));

            if (_isStereo) {
                // could be DUAL_OPL2 instead of OPL3
                // but i coded that is only OPL3 here...
                read(_alt_op_0, NUM_VOICES * sizeof(uint8_t));
                read(_alt_op_1, NUM_VOICES * sizeof(uint8_t));
                read(_conn_sel, NUM_VOICES * sizeof(uint8_t));
                read(_op4_voice, NUM_VOICES * sizeof(uint8_t));
                read(_carrier_01, 4 * sizeof(uint8_t));
                read(_carrier_23, 4 * sizeof(uint8_t));
            }


            int i = 0;
        }
    }
}
