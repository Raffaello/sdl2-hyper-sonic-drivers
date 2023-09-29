#pragma once

#include <memory>
#include <cstdint>
#include <list>

#include <HyperSonicDrivers/audio/midi/MIDIEvent.hpp>
#include <HyperSonicDrivers/audio/midi/types.hpp>
#include <HyperSonicDrivers/audio/opl/banks/OP2Bank.hpp>
#include <HyperSonicDrivers/devices/Opl.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplChannel.hpp>
#include <HyperSonicDrivers/drivers/midi/opl/OplVoice.hpp>
#include <HyperSonicDrivers/drivers/opl/OplWriter.hpp>
#include <HyperSonicDrivers/hardware/opl/OPL2instrument.h>
#include <HyperSonicDrivers/drivers/midi/IMidiDriver.hpp>


namespace HyperSonicDrivers::drivers::midi::opl
{
    /// <summary>
    /// OPL MidiDriver.
    /// Support OPL2 and OPL3 modes
    /// Mostly used to Play MUS File at the moment due to Op2Bank
    /// TODO: generalize the OP2Bank in OplBank (interface)
    /// TODO: support XMI extension.
    /// TODO: XMI Bank sounds
    /// </summary>
    class OplDriver : public IMidiDriver
    {
    public:
        explicit OplDriver(const std::shared_ptr<devices::Opl>& opl);
        ~OplDriver() override;

        bool open(const audio::mixer::eChannelGroup group,
            const uint8_t volume,
            const uint8_t pan) override;
        void close() override;

        inline void setOP2Bank(const std::shared_ptr<audio::opl::banks::OP2Bank>& op2Bank) noexcept { m_op2Bank = op2Bank; };

        void send(const audio::midi::MIDIEvent& e) /*const*/ noexcept override;
        void send(uint32_t msg) override { /*TODO*/ };
        void send(int8_t channel, uint32_t msg) override { /*TODO*/ }

        void pause() const noexcept override;
        void resume() const noexcept override;

        inline std::shared_ptr<hardware::opl::OPL> getOpl() const noexcept { return m_opl; };

    protected:
        void onCallback() noexcept override;

        // TODO: every MIDI function that has channel put into IMidiChannel interface
        // ------
        // MIDI Events
        void noteOff(const uint8_t chan, const uint8_t note) noexcept;
        void noteOn(const uint8_t chan, const uint8_t note, const uint8_t vol) noexcept;
        void controller(const uint8_t chan, const uint8_t ctrl, uint8_t value) noexcept;
        void programChange(const uint8_t chan, const uint8_t program) noexcept;
        void pitchBend(const uint8_t chan, const uint16_t bend) noexcept;

        // MIDI Controller Events
        void ctrl_modulationWheel(const uint8_t chan, const uint8_t value) const noexcept;
        void ctrl_volume(const uint8_t chan, const uint8_t value) const noexcept;
        void ctrl_panPosition(const uint8_t chan, uint8_t value) const noexcept;
        void ctrl_sustain(const uint8_t chan, uint8_t value) const noexcept;

    private:
        std::shared_ptr<hardware::opl::OPL> m_opl;
        std::shared_ptr<audio::opl::banks::OP2Bank> m_op2Bank;

        const bool m_opl3_mode;
        const uint8_t m_oplNumChannels;

        std::array<std::unique_ptr<OplChannel>, audio::midi::MIDI_MAX_CHANNELS>  m_channels;
        std::vector<std::unique_ptr<OplVoice>> m_voices;
        std::unique_ptr<drivers::opl::OplWriter> m_oplWriter;

        // TODO review to make this index more efficient (and its complementary)
        std::list<uint8_t> m_voicesInUseIndex;
        std::list<uint8_t> m_voicesFreeIndex;

        void releaseSustain(const uint8_t channel) const noexcept;
        uint8_t releaseVoice(const uint8_t slot, const bool forced);
        int allocateVoice(const uint8_t slot, const uint8_t channel,
            const uint8_t note, const uint8_t volume,
            const audio::opl::banks::Op2BankInstrument_t* instrument,
            const bool secondary);

        /// <summary>
        /// Get a Free OplVoice slot index if available.
        /// Otherwise it will release the first secondary voice found.
        /// Otherwise, if forced, will release the oldest voice.
        /// </summary>
        /// <param name="abs_time"></param>
        /// <param name="force">release the oldest channel if anything else is available</param>
        /// <returns></returns>
        int8_t getFreeOplVoiceIndex(const bool force);
    };
}
