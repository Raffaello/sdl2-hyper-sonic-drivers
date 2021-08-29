#pragma once

#include <files/File.hpp>
#include <cstdint>
#include <string>

namespace files
{
    template<typename T1, typename T2, typename T3, typename T4>
    constexpr uint32_t MKID_BE(T1 a, T2  b, T3  c, T4  d) { return a | b << 8 | c << 16 | d << 24; }
    
    enum class eIFF_ID : uint32_t
    {
        /* Amiga 8 bits voice */
        ID_FORM = MKID_BE('F', 'O', 'R', 'M'),
        /* EA IFF 85 group identifier */
        ID_CAT = MKID_BE('C', 'A', 'T', ' '),
        /* EA IFF 85 group identifier */
        ID_LIST = MKID_BE('L', 'I', 'S', 'T'),
        /* EA IFF 85 group identifier */
        ID_PROP = MKID_BE('P', 'R', 'O', 'P'),
        /* EA IFF 85 group identifier */
        ID_END = MKID_BE('E', 'N', 'D', ' '),
        /* unofficial END-of-FORM identifier (see Amiga RKM Devices Ed.3
           page 376) */
        ID_ILBM = MKID_BE('I', 'L', 'B', 'M'),
        /* EA IFF 85 raster bitmap form */
        ID_DEEP = MKID_BE('D', 'E', 'E', 'P'),
        /* Chunky pixel image files (Used in TV Paint) */
        ID_RGB8 = MKID_BE('R', 'G', 'B', '8'),
        /* RGB image forms, Turbo Silver (Impulse) */
        ID_RGBN = MKID_BE('R', 'G', 'B', 'N'),
        /* RGB image forms, Turbo Silver (Impulse) */
        ID_PBM = MKID_BE('P', 'B', 'M', ' '),
        /* 256-color chunky format (DPaint 2 ?) */
        ID_ACBM = MKID_BE('A', 'C', 'B', 'M'),
        /* Amiga Contiguous Bitmap (AmigaBasic) */
        ID_8SVX = MKID_BE('8', 'S', 'V', 'X'),
        /* Amiga 8 bits voice */

       /* generic */
       ID_FVER = MKID_BE('F', 'V', 'E', 'R'),
       /* AmigaOS version string */
       ID_JUNK = MKID_BE('J', 'U', 'N', 'K'),
       /* always ignore this chunk */
       ID_ANNO = MKID_BE('A', 'N', 'N', 'O'),
       /* EA IFF 85 Generic Annotation chunk */
       ID_AUTH = MKID_BE('A', 'U', 'T', 'H'),
       /* EA IFF 85 Generic Author chunk */
       ID_CHRS = MKID_BE('C', 'H', 'R', 'S'),
       /* EA IFF 85 Generic character string chunk */
       ID_NAME = MKID_BE('N', 'A', 'M', 'E'),
       /* EA IFF 85 Generic Name of art, music, etc. chunk */
       ID_TEXT = MKID_BE('T', 'E', 'X', 'T'),
       /* EA IFF 85 Generic unformatted ASCII text chunk */
       ID_copy = MKID_BE('(', 'c', ')', ' '),
       /* EA IFF 85 Generic Copyright text chunk */

       /* ILBM chunks */

       ID_BMHD = MKID_BE('B', 'M', 'H', 'D'),
       /* ILBM BitmapHeader */
       ID_CMAP = MKID_BE('C', 'M', 'A', 'P'),
       /* ILBM 8bit RGB colormap */
       ID_GRAB = MKID_BE('G', 'R', 'A', 'B'),
       /* ILBM "hotspot" coordiantes */
       ID_DEST = MKID_BE('D', 'E', 'S', 'T'),
       /* ILBM destination image info */
       ID_SPRT = MKID_BE('S', 'P', 'R', 'T'),
       /* ILBM sprite identifier */
       ID_CAMG = MKID_BE('C', 'A', 'M', 'G'),
       /* Amiga viewportmodes */
       ID_BODY = MKID_BE('B', 'O', 'D', 'Y'),
       /* ILBM image data */
       ID_CRNG = MKID_BE('C', 'R', 'N', 'G'),
       /* color cycling */
       ID_CCRT = MKID_BE('C', 'C', 'R', 'T'),
       /* color cycling */
       ID_CLUT = MKID_BE('C', 'L', 'U', 'T'),
       /* Color Lookup Table chunk */
       ID_DPI = MKID_BE('D', 'P', 'I', ' '),
       /* Dots per inch chunk */
       ID_DPPV = MKID_BE('D', 'P', 'P', 'V'),
       /* DPaint perspective chunk (EA) */
       ID_DRNG = MKID_BE('D', 'R', 'N', 'G'),
       /* DPaint IV enhanced color cycle chunk (EA) */
       ID_EPSF = MKID_BE('E', 'P', 'S', 'F'),
       /* Encapsulated Postscript chunk */
       ID_CMYK = MKID_BE('C', 'M', 'Y', 'K'),
       /* Cyan, Magenta, Yellow, & Black color map (Soft-Logik) */
       ID_CNAM = MKID_BE('C', 'N', 'A', 'M'),
       /* Color naming chunk (Soft-Logik) */
       ID_PCHG = MKID_BE('P', 'C', 'H', 'G'),
       /* Line by line palette control information (Sebastiano Vigna) */
       ID_PRVW = MKID_BE('P', 'R', 'V', 'W'),
       /* A mini duplicate ILBM used for preview (Gary Bonham) */
       ID_XBMI = MKID_BE('X', 'B', 'M', 'I'),
       /* eXtended BitMap Information (Soft-Logik) */
       ID_CTBL = MKID_BE('C', 'T', 'B', 'L'),
       /* Newtek Dynamic Ham color chunk */
       ID_DYCP = MKID_BE('D', 'Y', 'C', 'P'),
       /* Newtek Dynamic Ham chunk */
       ID_SHAM = MKID_BE('S', 'H', 'A', 'M'),
       /* Sliced HAM color chunk */
       ID_ABIT = MKID_BE('A', 'B', 'I', 'T'),
       /* ACBM body chunk */
       ID_DCOL = MKID_BE('D', 'C', 'O', 'L'),
       /* unofficial direct color */
       ID_DPPS = MKID_BE('D', 'P', 'P', 'S'),
       /* ? */
       ID_TINY = MKID_BE('T', 'I', 'N', 'Y'),
       /* ? */

       /* 8SVX chunks */

       ID_VHDR = MKID_BE('V', 'H', 'D', 'R'),
       /* 8SVX Voice8Header */

       ID_DATA = MKID_BE('D', 'A', 'T', 'A'),
       ID_DESC = MKID_BE('D', 'E', 'S', 'C'),
       ID_EMC2 = MKID_BE('E', 'M', 'C', '2'),
       ID_EVNT = MKID_BE('E', 'V', 'N', 'T'),
       ID_ICON = MKID_BE('I', 'C', 'O', 'N'),
       ID_INFO = MKID_BE('I', 'N', 'F', 'O'),
       ID_MENT = MKID_BE('M', 'E', 'N', 'T'),
       ID_ORDR = MKID_BE('O', 'R', 'D', 'R'),
       ID_RTBL = MKID_BE('R', 'T', 'B', 'L'),
       ID_RPAL = MKID_BE('R', 'P', 'A', 'L'),
       ID_SSET = MKID_BE('S', 'S', 'E', 'T'),
       ID_SINF = MKID_BE('S', 'I', 'N', 'F'),
       ID_TIMB = MKID_BE('T', 'I', 'M', 'B'),
       ID_RBRN = MKID_BE('R', 'B', 'R', 'N'),
    
       ID_XDIR = MKID_BE('X', 'D', 'I', 'R'),
       ID_XMID = MKID_BE('X', 'M', 'I', 'D'),
       ID_FILLER = MKID_BE(0, 0, 0, 0)
    };

    class IFFFile : public File
    {
    public:
        IFFFile(const std::string& filename);
        virtual ~IFFFile();
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
            IFF_ID   id;
            uint32_t size = 0;  // <! Big Endian
        } IFF_sub_chunk_header_t;
        static_assert(sizeof(IFF_sub_chunk_header_t) == 8);

        typedef struct IFF_chunk_header_t
        {
            IFF_sub_chunk_header_t chunk;
            IFF_ID                 type; 
        } IFF_chunk_header_t;
        static_assert(sizeof(IFF_chunk_header_t) == 12);

        void readChunkHeader(IFF_chunk_header_t& header);
        void readSubChunkHeader(IFF_sub_chunk_header_t& header);
        void readId(IFF_ID& iff_id);
    };
}
