#include <audio/opl/banks/OP2Bank.hpp>


namespace audio
{
    namespace opl
    {
        namespace banks
        {
            OP2Bank::OP2Bank(
                const std::array<Op2BankInstrument_t, OP2BANK_NUM_INSTRUMENTS> instruments,
                const std::array<std::string, OP2BANK_NUM_INSTRUMENTS> names
            ) : _instruments(instruments), _names(names)
            {
            }

            Op2BankInstrument_t OP2Bank::getInstrument(const uint8_t i) const
            {
                return _instruments.at(i);
            }

            std::string OP2Bank::getInstrumentName(const uint8_t i) const
            {
                return _names.at(i);
            }
        }
    }
}
