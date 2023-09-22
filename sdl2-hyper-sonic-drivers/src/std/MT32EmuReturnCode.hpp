#pragma once

#include <string>
#include <format>
#include <mt32emu/c_interface/cpp_interface.h>

namespace std
{
    template<>
    struct formatter<mt32emu_return_code> : formatter<string_view>
    {
        template<typename FormatContext>
        auto format(mt32emu_return_code ret, FormatContext& fc) const
        {
            string str;
            switch (ret)
            {
                case MT32EMU_RC_OK:
                    str = "OK";
                    break;
                case MT32EMU_RC_ADDED_CONTROL_ROM:
                    str = "ADDED_CONTROL_ROM";
                    break;
                case MT32EMU_RC_ADDED_PCM_ROM:
                    str = "ADDED_PCM_ROM";
                    break;
                case MT32EMU_RC_ADDED_PARTIAL_CONTROL_ROM:
                    str = "ADDED_PARTIAL_CONTROL_ROM";
                    break;
                case MT32EMU_RC_ADDED_PARTIAL_PCM_ROM:
                    str = "ADDED_PARTIAL_PCM_ROM";
                    break;
                case MT32EMU_RC_ROM_NOT_IDENTIFIED:
                    str = "ROM_NOT_IDENTIFIED";
                    break;
                case MT32EMU_RC_FILE_NOT_FOUND:
                    str = "FILE_NOT_FOUND";
                    break;
                case MT32EMU_RC_FILE_NOT_LOADED:
                    str = "FILE_NOT_LOADED";
                    break;
                case MT32EMU_RC_MISSING_ROMS:
                    str = "MISSING_ROMS";
                    break;
                case MT32EMU_RC_NOT_OPENED:
                    str = "NOT_OPENED";
                    break;
                case MT32EMU_RC_QUEUE_FULL:
                    str = "QUEUE_FULL";
                    break;
                case MT32EMU_RC_ROMS_NOT_PAIRABLE:
                    str = "ROMS_NOT_PAIRABLE";
                    break;
                case MT32EMU_RC_MACHINE_NOT_IDENTIFIED:
                    str = "MACHINE_NOT_IDENTIFIED";
                    break;
                case MT32EMU_RC_FAILED:
                    str = "FAILED";
                    break;
            }

            return formatter<string_view>::format(str, fc);
        }
    };
}
