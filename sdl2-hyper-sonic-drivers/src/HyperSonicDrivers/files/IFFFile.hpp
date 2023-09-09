#pragma once

#include <cstdint>
#include <string>
#include <HyperSonicDrivers/files/File.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::files
{
    enum class eIFF_ID : uint32_t
    {
        /* Amiga 8 bits voice */
        ID_FORM = utils::MKID_BE('F', 'O', 'R', 'M'),
        /* EA IFF 85 group identifier */
        ID_CAT = utils::MKID_BE('C', 'A', 'T', ' '),
        /* EA IFF 85 group identifier */
        ID_LIST = utils::MKID_BE('L', 'I', 'S', 'T'),
        /* EA IFF 85 group identifier */
        ID_PROP = utils::MKID_BE('P', 'R', 'O', 'P'),
        /* EA IFF 85 group identifier */
        ID_END = utils::MKID_BE('E', 'N', 'D', ' '),
        /* unofficial END-of-FORM identifier (see Amiga RKM Devices Ed.3
           page 376) */
        ID_ILBM = utils::MKID_BE('I', 'L', 'B', 'M'),
        /* EA IFF 85 raster bitmap form */
        ID_DEEP = utils::MKID_BE('D', 'E', 'E', 'P'),
        /* Chunky pixel image files (Used in TV Paint) */
        ID_RGB8 = utils::MKID_BE('R', 'G', 'B', '8'),
        /* RGB image forms, Turbo Silver (Impulse) */
        ID_RGBN = utils::MKID_BE('R', 'G', 'B', 'N'),
        /* RGB image forms, Turbo Silver (Impulse) */
        ID_PBM = utils::MKID_BE('P', 'B', 'M', ' '),
        /* 256-color chunky format (DPaint 2 ?) */
        ID_ACBM = utils::MKID_BE('A', 'C', 'B', 'M'),
        /* Amiga Contiguous Bitmap (AmigaBasic) */
        ID_8SVX = utils::MKID_BE('8', 'S', 'V', 'X'),
        /* Amiga 8 bits voice */

       /* generic */
       ID_FVER = utils::MKID_BE('F', 'V', 'E', 'R'),
       /* AmigaOS version string */
       ID_JUNK = utils::MKID_BE('J', 'U', 'N', 'K'),
       /* always ignore this chunk */
       ID_ANNO = utils::MKID_BE('A', 'N', 'N', 'O'),
       /* EA IFF 85 Generic Annotation chunk */
       ID_AUTH = utils::MKID_BE('A', 'U', 'T', 'H'),
       /* EA IFF 85 Generic Author chunk */
       ID_CHRS = utils::MKID_BE('C', 'H', 'R', 'S'),
       /* EA IFF 85 Generic character string chunk */
       ID_NAME = utils::MKID_BE('N', 'A', 'M', 'E'),
       /* EA IFF 85 Generic Name of art, music, etc. chunk */
       ID_TEXT = utils::MKID_BE('T', 'E', 'X', 'T'),
       /* EA IFF 85 Generic unformatted ASCII text chunk */
       ID_copy = utils::MKID_BE('(', 'c', ')', ' '),
       /* EA IFF 85 Generic Copyright text chunk */

       /* ILBM chunks */

       ID_BMHD = utils::MKID_BE('B', 'M', 'H', 'D'),
       /* ILBM BitmapHeader */
       ID_CMAP = utils::MKID_BE('C', 'M', 'A', 'P'),
       /* ILBM 8bit RGB colormap */
       ID_GRAB = utils::MKID_BE('G', 'R', 'A', 'B'),
       /* ILBM "hotspot" coordiantes */
       ID_DEST = utils::MKID_BE('D', 'E', 'S', 'T'),
       /* ILBM destination image info */
       ID_SPRT = utils::MKID_BE('S', 'P', 'R', 'T'),
       /* ILBM sprite identifier */
       ID_CAMG = utils::MKID_BE('C', 'A', 'M', 'G'),
       /* Amiga viewportmodes */
       ID_BODY = utils::MKID_BE('B', 'O', 'D', 'Y'),
       /* ILBM image data */
       ID_CRNG = utils::MKID_BE('C', 'R', 'N', 'G'),
       /* color cycling */
       ID_CCRT = utils::MKID_BE('C', 'C', 'R', 'T'),
       /* color cycling */
       ID_CLUT = utils::MKID_BE('C', 'L', 'U', 'T'),
       /* Color Lookup Table chunk */
       ID_DPI = utils::MKID_BE('D', 'P', 'I', ' '),
       /* Dots per inch chunk */
       ID_DPPV = utils::MKID_BE('D', 'P', 'P', 'V'),
       /* DPaint perspective chunk (EA) */
       ID_DRNG = utils::MKID_BE('D', 'R', 'N', 'G'),
       /* DPaint IV enhanced color cycle chunk (EA) */
       ID_EPSF = utils::MKID_BE('E', 'P', 'S', 'F'),
       /* Encapsulated Postscript chunk */
       ID_CMYK = utils::MKID_BE('C', 'M', 'Y', 'K'),
       /* Cyan, Magenta, Yellow, & Black color map (Soft-Logik) */
       ID_CNAM = utils::MKID_BE('C', 'N', 'A', 'M'),
       /* Color naming chunk (Soft-Logik) */
       ID_PCHG = utils::MKID_BE('P', 'C', 'H', 'G'),
       /* Line by line palette control information (Sebastiano Vigna) */
       ID_PRVW = utils::MKID_BE('P', 'R', 'V', 'W'),
       /* A mini duplicate ILBM used for preview (Gary Bonham) */
       ID_XBMI = utils::MKID_BE('X', 'B', 'M', 'I'),
       /* eXtended BitMap Information (Soft-Logik) */
       ID_CTBL = utils::MKID_BE('C', 'T', 'B', 'L'),
       /* Newtek Dynamic Ham color chunk */
       ID_DYCP = utils::MKID_BE('D', 'Y', 'C', 'P'),
       /* Newtek Dynamic Ham chunk */
       ID_SHAM = utils::MKID_BE('S', 'H', 'A', 'M'),
       /* Sliced HAM color chunk */
       ID_ABIT = utils::MKID_BE('A', 'B', 'I', 'T'),
       /* ACBM body chunk */
       ID_DCOL = utils::MKID_BE('D', 'C', 'O', 'L'),
       /* unofficial direct color */
       ID_DPPS = utils::MKID_BE('D', 'P', 'P', 'S'),
       /* ? */
       ID_TINY = utils::MKID_BE('T', 'I', 'N', 'Y'),
       /* ? */

       /* 8SVX chunks */

       ID_VHDR = utils::MKID_BE('V', 'H', 'D', 'R'),
       /* 8SVX Voice8Header */

       ID_DATA = utils::MKID_BE('D', 'A', 'T', 'A'),
       ID_DESC = utils::MKID_BE('D', 'E', 'S', 'C'),
       ID_EMC2 = utils::MKID_BE('E', 'M', 'C', '2'),
       ID_EVNT = utils::MKID_BE('E', 'V', 'N', 'T'),
       ID_ICON = utils::MKID_BE('I', 'C', 'O', 'N'),
       ID_INFO = utils::MKID_BE('I', 'N', 'F', 'O'),
       ID_MENT = utils::MKID_BE('M', 'E', 'N', 'T'),
       ID_ORDR = utils::MKID_BE('O', 'R', 'D', 'R'),
       ID_RTBL = utils::MKID_BE('R', 'T', 'B', 'L'),
       ID_RPAL = utils::MKID_BE('R', 'P', 'A', 'L'),
       ID_SSET = utils::MKID_BE('S', 'S', 'E', 'T'),
       ID_SINF = utils::MKID_BE('S', 'I', 'N', 'F'),
       ID_TIMB = utils::MKID_BE('T', 'I', 'M', 'B'),
       ID_RBRN = utils::MKID_BE('R', 'B', 'R', 'N'),
    
       ID_XDIR = utils::MKID_BE('X', 'D', 'I', 'R'),
       ID_XMID = utils::MKID_BE('X', 'M', 'I', 'D'),
       ID_FILLER = utils::MKID_BE(0, 0, 0, 0)
    };

    class IFFFile : public File
    {
    public:
        IFFFile(const std::string& filename);
        ~IFFFile() override = default;

    protected:
        typedef union
        {
            char     str[4];
            uint32_t value;
            eIFF_ID  id;
        } IFF_ID;
        static_assert(sizeof(IFF_ID) == sizeof(uint32_t));

        typedef struct IFF_sub_chunk_header_t
        {
            IFF_ID   id = { 0 };
            uint32_t size = 0;  // <! Big Endian
        } IFF_sub_chunk_header_t;
        static_assert(sizeof(IFF_sub_chunk_header_t) == 8);

        typedef struct IFF_chunk_header_t
        {
            IFF_sub_chunk_header_t chunk;
            IFF_ID                 type;
        } IFF_chunk_header_t;
        static_assert(sizeof(IFF_chunk_header_t) == 12);

        void readChunkHeader(IFF_chunk_header_t& header) const noexcept;
        void readSubChunkHeader(IFF_sub_chunk_header_t& header) const noexcept;
        void readId(IFF_ID& iff_id) const noexcept;
    };
}
