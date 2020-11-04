#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

using namespace Dive::StringHelper;

TEST(StringHelper, ToUppercase)
{
	ASSERT_STREQ("THIS IS A TEST STRING.", ToUppercase("This is A Test String.").c_str());

	const std::string str("THIS IS A TEST STRING.");
	ASSERT_STREQ("THIS IS A TEST STRING.", ToUppercase(str).c_str());
}

TEST(StringHelper, ToLowercase)
{
	ASSERT_STREQ("this is a test string.", ToLowercase("This is A Test String.").c_str());

	const std::string str("THIS IS A TEST STRING.");
	ASSERT_STREQ("this is a test string.", ToLowercase(str).c_str());
}

TEST(StringHelper, GetStringAfterToken)
{
	ASSERT_STREQ("love is great.", GetStringAfterToken("Our love is great.", "love").c_str());
}

TEST(StringHelper, GetStringBetweenTokens)
{
	ASSERT_STREQ(" Song: Plastic ", GetStringBetweenTokens("Next Song: Plastic Love.", "Next", "Love").c_str());
}

TEST(StringHelper, ReplaceToken)
{
	ASSERT_STREQ("Adobe Imageshop CC", ReplaceToken("Adobe Photoshop CC", "Photo", "Image").c_str());
}

TEST(StringHelper, StringFormatInt)
{
	int value = 1044;
	auto result = StringFormat("%d", value);
	ASSERT_STREQ("1044", result.c_str());
}

TEST(StringHelper, StringFormatDouble)
{
	double value = 184.321;
	auto result = StringFormat("%.3f", value);
	ASSERT_STREQ("184.321", result.c_str());
}

TEST(StringHelper, StringFormatString)
{
	std::string value = "StringHelper";
	auto result = StringFormat("%s", value.c_str());
	ASSERT_STREQ("StringHelper", result.c_str());
}

TEST(StringHelper, WStringFormatInt)
{
	int value = 1044;
	auto result = WStringFormat(L"%d", value);
	ASSERT_STREQ(L"1044", result.c_str());
}

TEST(StringHelper, WStringFormatDouble)
{
	double value = 184.321;
	auto result = WStringFormat(L"%.3f", value);
	ASSERT_STREQ(L"184.321", result.c_str());
}

TEST(StringHelper, WStringFormatWString)
{
	std::wstring value = L"StringHelper";
	auto result = WStringFormat(L"%s", value.c_str());
	ASSERT_STREQ(L"StringHelper", result.c_str());
}