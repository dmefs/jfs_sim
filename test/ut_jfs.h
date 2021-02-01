#pragma once


#include <gtest/gtest.h>
#include "../src/jfs.h"

class JFSTest : public ::testing::Test {
protected:
    void SetUp() override {
        jj = init_jfs(1);
    }

    void TearDown() override {
        end_jfs(jj);
    }

    jfs_t *jj;
};

TEST_F(JFSTest, InitJFS) {
    ASSERT_EQ(26214, jj->jarea.max_jarea_num);
    ASSERT_EQ(0, jj->jarea.jarea_size);
}
