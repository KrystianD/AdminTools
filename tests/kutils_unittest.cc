#include "../common/kutils.h"
#include "gtest/gtest.h"

TEST(KutilsTest, TestTrim) {
	ASSERT_EQ(trim("test \r\n"),"test");
	
	ASSERT_EQ(trim("test "),"test");
	ASSERT_EQ(trim(" test "),"test");
	ASSERT_EQ(trim(" test"),"test");
	
	ASSERT_EQ(trim("        test       "),"test");
}

TEST(KutilsTest, TestExplode) {
	vector<string> explodedParts;
	
	explodedParts = explode("test,explode",",");
	
	ASSERT_EQ(explodedParts[0],"test");
	ASSERT_EQ(explodedParts[1],"explode");
}