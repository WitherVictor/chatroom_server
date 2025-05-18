#include <gtest/gtest.h>

#include "utility.h"

TEST(valid_port_test, basic_test) {
    EXPECT_TRUE(utility::valid_port("8080"));
    EXPECT_TRUE(utility::valid_port("4242"));
    EXPECT_TRUE(utility::valid_port("52233"));
}

TEST(valid_port_test, negative_inputs) {
    EXPECT_FALSE(utility::valid_port("-53985"));
    EXPECT_FALSE(utility::valid_port("-1"));
}

TEST(valid_port_test, non_number_input) {
    EXPECT_FALSE(utility::valid_port("abcabc"));
    EXPECT_FALSE(utility::valid_port("8a0b8c0d"));
    EXPECT_FALSE(utility::valid_port("42abc24"));
}