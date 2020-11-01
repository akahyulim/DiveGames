#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

TEST(IniFileHelper, SetGetInt)
{
	auto file = Dive::FileSystemHelper::GetWorkingDirectory() + L"data.ini";

	Dive::IniFileHelper::SetInit(L"Display", L"width", 1600, file);
	EXPECT_EQ(1600, Dive::IniFileHelper::GetInt(L"Display", L"width", file));
}

TEST(IniFileHelper, SetGetFloat)
{
	auto file = Dive::FileSystemHelper::GetWorkingDirectory() + L"data.ini";

	Dive::IniFileHelper::SetFloat(L"Display", L"far plane", 1000.0, file);
	EXPECT_FLOAT_EQ(1000.0f, Dive::IniFileHelper::GetFloat(L"Display", L"far plane", file));
}

TEST(IniFileHelper, SetGetBool)
{
	auto file = Dive::FileSystemHelper::GetWorkingDirectory() + L"data.ini";

	Dive::IniFileHelper::SetBool(L"Display", L"vSync", true, file);
	EXPECT_TRUE( Dive::IniFileHelper::GetBool(L"Display", L"vSync", file));
}

TEST(IniFileHelper, SetGetStr)
{
	auto file = Dive::FileSystemHelper::GetWorkingDirectory() + L"data.ini";

	Dive::IniFileHelper::SetStr(L"Info", L"name", L"knave", file);
	//utf-8로 변경 후???
	//EXPECT_STREQ(L"knave", Dive::IniFileHelper::GetStr(L"Info", L"name", file));
}