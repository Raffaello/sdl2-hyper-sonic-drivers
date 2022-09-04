#pragma once

namespace hardware
{
    namespace opl
    {
        /* OPL2 instrument */
        typedef struct OPL2instrument_t
        {
            /*00*/  uint8_t     trem_vibr_1;    /* OP 1: tremolo/vibrato/sustain/KSR/multi */
            /*01*/  uint8_t     att_dec_1;      /* OP 1: attack rate/decay rate */
            /*02*/  uint8_t     sust_rel_1;     /* OP 1: sustain level/release rate */
            /*03*/  uint8_t     wave_1;         /* OP 1: waveform select */
            /*04*/  uint8_t     scale_1;        /* OP 1: key scale level */
            /*05*/  uint8_t     level_1;        /* OP 1: output level */
            /*06*/  uint8_t     feedback;       /* feedback/AM-FM (both operators) */
            /*07*/  uint8_t     trem_vibr_2;    /* OP 2: tremolo/vibrato/sustain/KSR/multi */
            /*08*/  uint8_t     att_dec_2;      /* OP 2: attack rate/decay rate */
            /*09*/  uint8_t     sust_rel_2;     /* OP 2: sustain level/release rate */
            /*0A*/  uint8_t     wave_2;         /* OP 2: waveform select */
            /*0B*/  uint8_t     scale_2;        /* OP 2: key scale level */
            /*0C*/  uint8_t     level_2;        /* OP 2: output level */
            /*0D*/  uint8_t     unused;
            /*0E*/  uint16_t    basenote;       /* base note offset */
        } OPL2instrument_t;
    }
}
