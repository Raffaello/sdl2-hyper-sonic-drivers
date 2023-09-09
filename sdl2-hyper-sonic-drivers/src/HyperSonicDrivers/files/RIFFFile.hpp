#pragma once

#include <string>
#include <cstdint>
#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::files
{
    enum class eRIFF_ID : uint32_t
    {
        ID_RIFF = utils::MKID_BE('R', 'I', 'F', 'F'),
        ID_PAL = utils::MKID_BE('P', 'A', 'L', ' '),
        ID_RDIB = utils::MKID_BE('R', 'D', 'I', 'B'),
        ID_RMID = utils::MKID_BE('R', 'M', 'I', 'D'), // RIFF MIDI
        ID_RMMP = utils::MKID_BE('R', 'M', 'M', 'P'),
        ID_WAVE = utils::MKID_BE('W', 'A', 'V', 'E'), // WAVE
        ID_LIST = utils::MKID_BE('L', 'I', 'S', 'T'),
        ID_IARL = utils::MKID_BE('I', 'A', 'R', 'L'),
        ID_IART = utils::MKID_BE('I', 'A', 'R', 'T'),
        ID_ICMS = utils::MKID_BE('I', 'C', 'M', 'S'),
        ID_ICMT = utils::MKID_BE('I', 'C', 'M', 'T'),
        ID_ICOP = utils::MKID_BE('I', 'C', 'O', 'P'),
        ID_ICRD = utils::MKID_BE('I', 'C', 'R', 'D'),
        ID_ICRP = utils::MKID_BE('I', 'C', 'R', 'P'),
        ID_IDIM = utils::MKID_BE('I', 'D', 'I', 'M'),
        ID_IDPI = utils::MKID_BE('I', 'D', 'P', 'I'),
        ID_IENG = utils::MKID_BE('I', 'E', 'N', 'G'),
        ID_IGNR = utils::MKID_BE('I', 'G', 'N', 'R'),
        ID_IKEY = utils::MKID_BE('I', 'K', 'E', 'Y'),
        ID_ILGT = utils::MKID_BE('I', 'L', 'G', 'T'),
        ID_IMED = utils::MKID_BE('I', 'D', 'E', 'D'),
        ID_INAM = utils::MKID_BE('I', 'N', 'A', 'M'),
        ID_IPLT = utils::MKID_BE('I', 'P', 'L', 'T'),
        ID_IPRD = utils::MKID_BE('I', 'P', 'R', 'D'),
        ID_ISBJ = utils::MKID_BE('I', 'S', 'B', 'J'),
        ID_ISFT = utils::MKID_BE('I', 'S', 'F', 'T'),
        ID_ISHP = utils::MKID_BE('I', 'S', 'H', 'P'),
        ID_ISRC = utils::MKID_BE('I', 'S', 'R', 'C'),
        ID_ISRF = utils::MKID_BE('I', 'S', 'R', 'F'),
        ID_ITCH = utils::MKID_BE('I', 'T', 'C', 'H'),
        ID_CSET = utils::MKID_BE('C', 'S', 'E', 'T'),
        ID_JUNK = utils::MKID_BE('J', 'U', 'N', 'K'),
        ID_CTOC = utils::MKID_BE('C', 'T', 'O', 'C'),
        ID_CGRP = utils::MKID_BE('C', 'G', 'R', 'P'),
        ID_FMT  = utils::MKID_BE('f', 'm', 't', ' '),
        ID_DATA = utils::MKID_BE('d', 'a', 't', 'a')
    };

    class RIFFFile : protected File
    {
    public:
        RIFFFile(const std::string& filename);
        ~RIFFFile() override = default;
    protected:
        typedef union
        {
            char     str[4];
            uint32_t value;
            eRIFF_ID  id;
        } RIFF_ID;
        static_assert(sizeof(RIFF_ID) == sizeof(uint32_t));

        typedef struct RIFF_sub_chunk_header_t
        {
            RIFF_ID  id;
            uint32_t length;
        } RIFF_sub_chunk_header_t;
        static_assert(sizeof(RIFF_sub_chunk_header_t) == 8);

        typedef struct RIFF_chunk_header_t
        {
            RIFF_sub_chunk_header_t chunk;
            RIFF_ID                 type;
        } RIFF_chunk_header_t;
        static_assert(sizeof(RIFF_chunk_header_t) == 12);

        void readChunkHeader(RIFF_chunk_header_t& header) const noexcept;
        void readSubChunkHeader(RIFF_sub_chunk_header_t& header) const noexcept;
        void readId(RIFF_ID& iff_id) const noexcept;
    };
}
