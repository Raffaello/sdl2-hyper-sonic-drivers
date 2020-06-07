# Sonic Drivers

`sdl2-sonic-drivers` is a `sdl2_mixer` based emulating audio of '90s hardware PCs (adlib, OPL3, ...)

reading some old files too as utility and give the capability to render them in digital format
or play them in real time

## Credits

The majority of the core code is part of these projects:

- ScummVM
- Mame
- DosBox
	

## Notes

TODO

- `drivers` folder is analog to `*.DRV`|`*.ADV` files. (to read them or to be hardcoded)
- `hardware` is the soundcard/chip emulation.
- `audio`  API interface?
- `files` `*.ADL` `*.XMI` `*.MID`

## TODO

- Adlib
- OPL2
- OPL3
- GM
- GUS
- PC Speaker
- XMI format (xmidi)
- render to digital wave or mp3
