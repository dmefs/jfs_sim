#pragma once

#include <gtest/gtest.h>
#include "../src/jfs.h"

class CommandTableTest : public ::testing::Test {
protected:
    void SetUp() override {

    }

    void TearDown() override {

    }

};

TEST_F(CommandTableTest, InitJFS) {
    init_jfs(1);
}

