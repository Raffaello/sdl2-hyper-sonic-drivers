#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/files/dmx/OP2File.hpp>
#include <HyperSonicDrivers/audio/stubs/StubMixer.hpp>
#include <HyperSonicDrivers/audio/mixer/ChannelGroup.hpp>
#include <HyperSonicDrivers/hardware/opl/OplType.hpp>
#include <HyperSonicDrivers/hardware/opl/OPLFactory.hpp>
#include <stdexcept>

namespace HyperSonicDrivers::devices::midi
{
    using audio::stubs::StubMixer;
    using hardware::opl::OplType;
    using hardware::opl::OplEmulator;
    using hardware::opl::OPLFactory;
    using files::dmx::OP2File;
    using audio::mixer::eChannelGroup;

    static const std::string GENMIDI_OP2 = std::string("../fixtures/GENMIDI.OP2");

    template<class T>
    class EmulatorTestCase : public ::testing::TestWithParam<std::tuple<hardware::opl::OplEmulator, bool>>
    {
    public:
        const hardware::opl::OplEmulator oplEmu = std::get<0>(GetParam());
        const bool shouldThrow = std::get<1>(GetParam());
        const files::dmx::OP2File op2File = OP2File(GENMIDI_OP2);
        const std::shared_ptr<audio::stubs::StubMixer> mixer = std::make_shared<audio::stubs::StubMixer>();

        void test_case()
        {
            using audio::mixer::eChannelGroup;

            if (this->shouldThrow)
            {
                EXPECT_THROW(
                    T(this->mixer, this->op2File.getBank(), eChannelGroup::Plain, this->oplEmu),
                    std::runtime_error
                );
            }
            else {
                EXPECT_NO_THROW(T(this->mixer, this->op2File.getBank(), eChannelGroup::Plain, this->oplEmu));
            }
        }
    };
}
