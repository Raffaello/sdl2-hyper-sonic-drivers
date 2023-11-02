#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <HyperSonicDrivers/hardware/opl/OplEmulator.hpp>
#include <HyperSonicDrivers/audio/IMixerMock.hpp>
#include <stdexcept>

namespace HyperSonicDrivers::devices
{
    using audio::IMixerMock;
    using hardware::opl::OplEmulator;

    template<class T>
    class EmulatorTestCase : public ::testing::TestWithParam<std::tuple<OplEmulator, bool>>
    {
    public:
        const OplEmulator oplEmu = std::get<0>(GetParam());
        const bool shouldFail = std::get<1>(GetParam());
        const std::shared_ptr<IMixerMock> mixer = std::make_shared<IMixerMock>();

        void test_case()
        {
            T opl(this->mixer, this->oplEmu);
            EXPECT_EQ(opl.init(), !this->shouldFail);
        }
    };
}
