#include <gtest/gtest.h>

#include "utility.hpp"

TEST(valid_port_test, basic_test) {
    EXPECT_TRUE(valid_port("8080"));
    EXPECT_TRUE(valid_port("4242"));
    EXPECT_TRUE(valid_port("52233"));
}

TEST(valid_port_test, negative_inputs) {
    EXPECT_FALSE(valid_port("-53985"));
    EXPECT_FALSE(valid_port("-1"));
}

TEST(valid_port_test, non_number_input) {
    EXPECT_FALSE(valid_port("abcabc"));
    EXPECT_FALSE(valid_port("8a0b8c0d"));
    EXPECT_FALSE(valid_port("42abc24"));
}