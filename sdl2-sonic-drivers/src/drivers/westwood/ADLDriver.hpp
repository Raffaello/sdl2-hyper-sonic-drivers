#pragma once

namespace drivers
{
    namespace westwood
    {
       
        /// <summary>
        /// Driver for .ADL files and OPL Chips
        /// Originally shuold be the DUNE2 ALFX.DRV file and PCSOUND.DRV
        /// This file was propretary for optimized Westwood .ADL files
        /// and they were not using Miles driver for musics in OPL Chips
        /// as those were only for .XMI files and only used for MT-32/GM
        /// ------------------------------------------------------------
        /// AdLib implementation of the sound output device.
        ///
        /// It uses a sound file format special to EoB I, II, Dune II,
        /// Kyrandia 1 and 2, and LoL.There are slightly different
        /// variants: EoB I uses the oldest format(version 1);
        /// EoB II(version 2), Dune IIand Kyrandia 1 (version 3) have
        /// the same file format(but need different offset adjustments);
        /// Kyrandia 2 and LoL format(version 4) is different again.
        /// </summary>
        class ADLDriver
        {

         
        };
    }
}
