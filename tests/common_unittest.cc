#include "../common/common.h"
#include "gtest/gtest.h"

TEST(CommonTest, TestFileExists) {
	ASSERT_EQ(fileExists("assets/existing_file.txt"), true);

	ASSERT_EQ(fileExists("assets/not_existing_file.txt"), false);
}

TEST(CommonTest, TestFileAccessible) {
	ASSERT_EQ(fileAccessible("assets/accessible_file"), true);

	ASSERT_EQ(fileAccessible("assets/inaccesible_file"), false);
}