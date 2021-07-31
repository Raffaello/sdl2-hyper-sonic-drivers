#pragma once

#include <cstdint>
#include <cmath>

namespace hardware
{
    namespace opl
    {
        // TODO: replace intptr_t and uintptr_t with something more specific?
        namespace woodyopl
        {
            constexpr int NUM_CHANNELS = 9;
            constexpr int MAXCELLS = NUM_CHANNELS * 2;

            // not used..
            constexpr double FL0 = 0.0;
            constexpr double FL05 = 0.5;
            constexpr double FL1 = 1.0;
            constexpr double FL2 = 2.0;
            // TODO: consider to put constexpr constant values like PI into a shared hpp file.
#ifndef M_PI
            constexpr double M_PI = 3.14159265358979323846;
#endif // ! M_PI
            constexpr double PI   = 3.141592653589793238462643;

            constexpr int WAVPREC = 1024;
            constexpr double AMPVOL = 8192.0 / 2.0;
            //#define FRQSCALE      (49716/512.0)
            constexpr int INTFREQU = 50000;
            constexpr double MODFACTOR = (WAVPREC * 4.0);      //How much of modulator cell goes into carrier

            // TODO: CF_TYPE can be converted to enum (class)
            constexpr int CF_TYPE_ATT = 0;
            constexpr int CF_TYPE_DEC = 1;
            constexpr int CF_TYPE_REL = 2;
            constexpr int CF_TYPE_SUS = 3;
            constexpr int CF_TYPE_SUS_NOKEEP = 4;
            constexpr int CF_TYPE_OFF = 5;

            // TODO: ARC_ can be converted to enum (class)
            constexpr int ARC_CONTROL = 0x00;
            constexpr int ARC_TVS_KSR_MUL = 0x20;
            constexpr int ARC_KSL_OUTLEV = 0x40;
            constexpr int ARC_ATTR_DECR = 0x60;
            constexpr int ARC_SUSL_RELR = 0x80;
            constexpr int ARC_FREQ_NUM = 0xa0;
            constexpr int ARC_KON_BNUM = 0xb0;
            constexpr int ARC_PERC_MODE = 0xbd;
            constexpr int ARC_FEEDBACK = 0xc0;
            constexpr int ARC_WAVE_SEL = 0xe0;
            constexpr int ARC_SECONDSET = 0x100;

            constexpr int FIFOSIZE = 512;

            // vibrato constants
            constexpr int VIBTAB_SIZE = 8;
            constexpr double VIB_FREQ = INTFREQU / 8192.0; // vibrato at 6.1hz ?? (opl3 docs say 6.1, opl4 docs say 6.0, y8950 docs say 6.4)

            // tremolo constants and table
            constexpr int TREMTAB_SIZE = 53;
            constexpr double TREM_FREQ = 3.7; // tremolo at 3.7hz

            /// <summary>
            /// cell struct definition
            ///    For OPL2 all 9 channels consist of two cells each, carrier and modulator.
            ///    Channel x has cell x as modulator and cell (9+x) as carrier.
            ///    For OPL3 all 18 channels consist either of two cells (2op mode) or four
            ///    cells (4op mode) which is determined through register4 of the second
            ///    adlib register set.
            ///    Only the channels 0,1,2 (first set) and 9,10,11 (second set) can act as
            ///    4op channels. The two additional operators (cells) for a channel y come
            ///    from the 2op channel y+3 so the cells y, (9+y), y+3, (9+y)+3 make up a 4op
            ///    channel.
            /// </summary>
            typedef struct cell_type_struct
            {
                double val, lastval;            // current output/last output (used for feedback)
                double t, tinc;                 // time (position in waveform) and time increment
                double vol, amp, step_amp;      // volume and amplification (cell envelope)
                double sustain_level, mfb;      // sustain level, feedback amount
                double a0, a1, a2, a3;          // attack rate function coefficients
                double decaymul, releasemul;    // decay/release rate functions
                uintptr_t cf_sel;                    // current state of cell (attack/decay/sustain/release/off)
                intptr_t toff;
                intptr_t freq_high;                 // highest three intptr_t of the frequency, used for vibrato calculations
                int16_t* cur_wform;              // start of selected waveform
                uintptr_t cur_wmask;                 // mask for selected waveform
                bool sus_keep;                  // keep sustain level when decay finished
                bool vibrato, tremolo;           // vibrato/tremolo enable intptr_t

                // variables used to provide non-continuous envelopes
                double generator_pos;           // for non-standard sample rates we need to determine how many samples have passed
                intptr_t cur_env_step;              // current (standardized) sample position
                intptr_t env_step_a, env_step_d, env_step_r;  // number of std samples of one step (for attack/decay/release mode)
                uint8_t step_skip_pos;            // position of 8-cyclic step skipping (always 2^x to check against mask)
                intptr_t env_step_skip_a;           // bitmask that determines if a step is skipped (respective bit is zero then)

            } celltype;

            uintptr_t adlib_reg_read(uintptr_t oplnum, uintptr_t port);
            void adlib_reg_write(uintptr_t oplnum, uintptr_t port, uint8_t val);

            class OPLChip
            {
                //TODO: default constructor need to call adlib_init
            public:
                // per-chip variables
                //uintptr_t chip_num;      /// ????
                celltype cell[MAXCELLS];

                intptr_t int_samplerate, ext_samplerate;

                uint8_t status, index;
                uint8_t adlibreg[256];    // adlib register set
                uint8_t wave_sel[22];     // waveform selection
                uint8_t timer[2];

                // vibrato/tremolo increment/counter
                double vibtab_pos;
                double vibtab_add;
                double tremtab_pos;
                double tremtab_add;

                //OPLChip(const uintptr_t cnum) noexcept;

                // enable a cell
                void cellon(uintptr_t regbase, celltype* c);

                // functions to change parameters of a cell
                void change_cellfreq(uintptr_t chanbase, uintptr_t regbase, celltype* c);

                void change_attackrate(uintptr_t regbase, celltype* c);
                void change_decayrate(uintptr_t regbase, celltype* c);
                void change_releaserate(uintptr_t regbase, celltype* c);
                void change_sustainlevel(uintptr_t regbase, celltype* c);
                void change_waveform(uintptr_t regbase, celltype* c);
                void change_keepsustain(uintptr_t regbase, celltype* c);
                void change_vibrato(uintptr_t regbase, celltype* c);

                void change_feedback(uintptr_t chanbase, celltype* c);

                // general functions
                void adlib_init(intptr_t samplerate);
                void adlib_write(uintptr_t idx, uint8_t val, uintptr_t second_set);
                void adlib_getsample(int16_t* sndptr, intptr_t numsamples);
            };

            extern OPLChip* oplchip[2];
        }
    }
}
