#include <HyperSonicDrivers/drivers/midi/mt32/MT32Driver.hpp>
#include <HyperSonicDrivers/utils/ILogger.hpp>

namespace HyperSonicDrivers::drivers::midi::mt32
{
    MT32Driver::MT32Driver(const std::shared_ptr<devices::MT32>& mt32) :
        IMidiDriver()
    {
        if(mt32 == nullptr)
            utils::throwLogC<std::runtime_error>("MT32 device is null");

        m_mt32 = mt32->getMt32();
    }

    MT32Driver::~MT32Driver()
    {
        // TODO: this can be done in the parent class using IHardware pointer.
        MT32Driver::close();
    }

    bool MT32Driver::open(const audio::mixer::eChannelGroup group, const uint8_t volume, const uint8_t pan)
    {
        if (isOpen())
            return true;

        if (!m_mt32->init())
            return false;

        // TODO: mostly the same as for OPL
        hardware::TimerCallBack cb = std::bind_front(&MT32Driver::onCallback, this);
        auto p = std::make_shared<hardware::TimerCallBack>(cb);
        m_mt32->start(p, group, volume, pan);

        m_isOpen = true;
        return true;
    }

    void MT32Driver::close()
    {
        // TODO: this can be done in the parent class using IHardware pointer
        if (isOpen())
        {
            m_mt32->stop();
            m_isOpen = false;
        }
    }

    void MT32Driver::send(const uint32_t msg) noexcept
    {
        // NOTE/TODO: the mt32Emu library could directly play midi
        m_mt32->getService().playMsg(msg);
    }

    void MT32Driver::onCallback() noexcept
    {
    }
}
