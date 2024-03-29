#include <HyperSonicDrivers/utils/opl.hpp>
#include <HyperSonicDrivers/utils/algorithms.hpp>

namespace HyperSonicDrivers::utils
{
    void FMoutput(const uint32_t port, const uint8_t reg, const uint8_t val, const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        opl->write(port, reg);
        //wait 8 microsec;
        utils::delayMicro(8);
        opl->write(port + 1, val);
        //wait 55 microsec
        utils::delayMicro(55);
    }

    void fm(const uint8_t reg, const uint8_t val, const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        FMoutput(FM, reg, val, opl);
    }

    void Profm1(const uint8_t reg, const uint8_t val, const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        FMoutput(PROFM1, reg, val, opl);
    }

    void Profm2(const uint8_t reg, const uint8_t val, const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        FMoutput(PROFM2, reg, val, opl);
    }

    bool detectOPL2(const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        if (!opl->isInit()) {
                return false;
        }

        // Note: Steps 1 and 2 can't be combined together.
        // Reset Timer 1 and Timer 2: write 60h to register 4.
        fm(4, 0x60, opl);
        // Reset the IRQ: write 80h to register 4.
        fm(4, 0x80, opl);
        // Read status register: read port base+0 (388h). Save the result.
        uint8_t status1 = opl->read(0) & 0xE0;
        // Set Timer 1 to FFh: write FFh to register 2.
        fm(2, 0xFF, opl);
        // Unmask and start Timer 1: write 21h to register 4.
        fm(4, 0x21, opl);
        // Wait in a delay loop for at least 80 usec.
        utils::delayMillis(100);

        // Read status register: read port base+0 (388h). Save the result.
        uint8_t status2 = opl->read(0) & 0xE0;

        // Reset Timer 1, Timer 2 and IRQ as in steps 1 and 2.
        fm(4, 0x60, opl);
        fm(4, 0x80, opl);

        // Test the results of the two reads: the first should be 0, the second should be C0h. If either is incorrect, then the OPL2 is not present.
        return status1 == 0 && status2 == 0xC0;
    }

    bool detectOPL3(const std::shared_ptr<hardware::opl::OPL>& opl)
    {
        // Detect OPL2. If present, continue.
        if (!detectOPL2(opl))
            return false;

        // AND the result with 06h. (or E0h?)
        // If the result is zero, you have OPL3, otherwise OPL2.
        bool status = (opl->read(0) &0x6) == 0;

        return status;
    }
}
