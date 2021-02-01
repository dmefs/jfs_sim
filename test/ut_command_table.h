#pragma once

#include <gtest/gtest.h>
#include "../src/command_table.h"

class CommandTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        init_command_table(&head, 3);
    }

    void TearDown() override {
        end_command_table(&head);
    }
    transaction_head_t head;
};

TEST_F(CommandTableTest, InitCommandTable) {
    ASSERT_EQ(3, head.capacity);
    ASSERT_EQ(0, head.size);
    ASSERT_EQ(false, head.is_full(&head));
}

TEST_F(CommandTableTest, AddCommandTable) {
    add_command_table(&head, 100, 1, 0);
    ASSERT_EQ(3, head.capacity);
    ASSERT_EQ(1, head.size);
    ASSERT_EQ(false, head.is_full(&head));
    add_command_table(&head, 200, 1, 1);
    ASSERT_EQ(3, head.capacity);
    ASSERT_EQ(2, head.size);
    ASSERT_EQ(false, head.is_full(&head));
}

TEST_F(CommandTableTest, ExtendCommandTable) {
    add_command_table(&head, 100, 1, 0);
    add_command_table(&head, 200, 1, 1);
    add_command_table(&head, 400, 1, 2);
    ASSERT_EQ(true, head.is_full(&head));
    ASSERT_EQ(3, head.size);
    add_command_table(&head, 300, 1, 3);
    ASSERT_EQ(6, head.capacity);
    ASSERT_EQ(4, head.size);
}