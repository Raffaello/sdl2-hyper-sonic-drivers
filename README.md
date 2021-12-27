[![Codacy Badge](https://app.codacy.com/project/badge/Grade/94519cf7a54248bf96e3de83cc3cc897)](https://www.codacy.com/gh/Raffaello/sdl2-sonic-drivers/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Raffaello/sdl2-sonic-drivers&amp;utm_campaign=Badge_Grade)

![windows](https://github.com/raffaello/sdl2-sonic-drivers/actions/workflows/ci-windows.yml/badge.svg?branch=master)

# Sonic Drivers

`sdl2-sonic-drivers` is a `sdl2` based emulating audio of '90s hardware PCs (adlib, OPL3, ...)

reading some old files too as utility and give the capability to render them in digital format
or play them in real time

## Digital sound files
Partially reading these types of files, depending of their format.
- [x] VOC files
- [x] WAV files


## Music sound files

- [x] ADL files
- [ ] XMI files
- [ ] MID files

## Emulators
At the moment the only implemented emulators are OPL chips.

### Adlib/Sound Blaster PRO 2.0 chips
- [x] Mame OPL2
- [/] Mame OPL3
- [x] DosBox OPL2
- [x] DosBox Dual OPL2
- [/] DosBox OPL3
- [x] Nuked OPL3
- [x] "woody" OPL2/Surround OPL2 (adding a sort of chrous effect)


## Credits

Most of the core emulation code is part of these projects:

- [ScummVM](https://www.scummvm.org/) mostly all emulators
- [Mame](https://www.mamedev.org/) 
- [DosBox](https://www.dosbox.com/)
- [AIL](http://www.vgmpf.com/Wiki/index.php?title=Audio_Interface_Library) reverse engineering


## Notes

## TODO

- decouple emultation to SDL2
- Adlib (OPL2, dualOPL2, OPL3)
- GM
- GUS
- PC Speaker (8 bit, 16 bit, 32-bit processing)
- XMI format (xmidi) (Westwood .PCS .C55 .TAN are XMI files)
- render to digital wave or mp3
