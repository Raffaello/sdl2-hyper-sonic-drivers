#pragma once

#include <cstdint>

namespace HyperSonicDrivers::drivers::midi
{
    class IMidiChannel;

    /**
    * Interface for Midi Channel Voice message to have a polyphonic MidiChannel.
    * this is mono-phonic channel, multiple combination of this gives a polyphonic MidiChannel
    **/
    class IMidiChannelVoice
    {
    public:
        IMidiChannelVoice() = default;
        virtual ~IMidiChannelVoice() = default;

        inline IMidiChannel* getChannel() const noexcept { return m_channel; }
        uint8_t getChannelNum() const noexcept;
        inline uint8_t getNote() const noexcept { return m_note; }
        inline uint8_t getVolume() const noexcept { return m_volume; };
        void setVolumes(const uint8_t volume) noexcept;
        inline bool isFree() const noexcept { return m_free; }
        inline bool isVibrato() const noexcept { return m_vibrato; }
        
    protected:
        IMidiChannel* m_channel = nullptr;  // MIDI channel
        uint8_t m_note = 0;                 /* note number */
        uint8_t m_volume = 0;               /* note volume */
        uint8_t m_real_volume = 0;          /* adjusted note volume */
        int16_t m_pitch_factor = 0;         /* pitch-wheel value */
        bool m_free = true;
        bool m_sustain = false; // this are Opl exclusive or are midi?
        bool m_vibrato = false; // ""
 
    private:
        /// <summary>
        /// The volume is between 0-127 as a per MIDI specification.
        /// OPLWriter expect a MIDI volume value and converts to OPL value.
        /// OPL chips has a volume attenuation (inverted values)
        /// range from 0-64 inverted (0 is max, 64 is muted).
        /// </summary>
        uint8_t calcVolume_() const noexcept;
    };

}
