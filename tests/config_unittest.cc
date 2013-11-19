#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "../common/config.h"
#include "gtest/gtest.h"

TEST(ConfigTest, TestHasKey) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(cfg.hasKey("db"), true);
	ASSERT_EQ(cfg.hasKey("port"), true);
	ASSERT_EQ(cfg.hasKey("db_undeclared"), false);
}

TEST(ConfigTest, TestGettingString) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(typeid(cfg.getString("db", "undeclared")), typeid(string));
}

TEST(ConfigTest, TestGettingDeclaredString) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(cfg.getString("db", "undeclared"), "data.db");
	ASSERT_NE(cfg.getString("db", "undeclared"), "undeclared");
}

TEST(ConfigTest, TestGettingUndeclaredString) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(cfg.getString("db_undeclared", "undeclared"), "undeclared");
	ASSERT_NE(cfg.getString("db_undeclared", "undeclared"), "data.db");
}

TEST(ConfigTest, TestGettingInt) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(typeid(cfg.getInt("port", 0)), typeid(int));
}

TEST(ConfigTest, TestGettingDeclaredInt) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(cfg.getInt("port", 0), 1234);
	ASSERT_NE(cfg.getInt("port", 0), 0);
}

TEST(ConfigTest, TestGettingUndeclaredInt) {
	Config cfg;
	cfg.fromFile("assets/correct_config_file.txt");
	
	ASSERT_EQ(cfg.getInt("port_undeclared", 0), 0);
	ASSERT_NE(cfg.getInt("port_undeclared", 0), 1234);
}

std::string replaceAll (const string& str, const string& what, const string& replacement)
{
	string newStr = "";
	size_t idx = 0;
	size_t pos;
	while (idx < str.size ())
	{
		pos = str.find (what, idx);
		if (pos == string::npos)
		{
			newStr += str.substr (idx);
			break;
		}
		else
		{
			newStr += str.substr (idx, pos - idx) + replacement;
			idx = pos + what.size ();
		}
	}
	return newStr;
}


TEST(ConfigTest, TestSaveToFile) {
	// clean file contents
	std::ofstream ofs;
	ofs.open ("assets/test_save_to_file", std::ofstream::out);
	ofs.close();

	// get config object, set values and save it to file
	Config cfg;

	cfg.setInt("port", 1234);	
	cfg.setString("db", "data.db");

	cfg.saveToFile("assets/test_save_to_file");
	
	// get pattern file and saved file
	std::ifstream newFile("assets/test_save_to_file");
	std::stringstream bufferNewFile;
	bufferNewFile << newFile.rdbuf();

	std::ifstream patternFile("assets/good_config_pattern");
	std::stringstream bufferPatternFile;
	bufferPatternFile << patternFile.rdbuf();
	
	// compare two files, they should be equal
	ASSERT_EQ(replaceAll(bufferPatternFile.str(),"\r","").compare(replaceAll(bufferNewFile.str(),"\r","")), 0);
}
