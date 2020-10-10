#include "GoogleTest/include/gtest/gtest.h"
#include "Core/StringUtils.h"

TEST(StringUtils, ToUppercase)
{
	ASSERT_STREQ("THIS IS A TEST STRING.", Dive::ToUppercase("This is A Test String.").c_str());

	const std::string str("THIS IS A TEST STRING.");
	ASSERT_STREQ("THIS IS A TEST STRING.", Dive::ToUppercase(str).c_str());
}

TEST(StringUtils, ToLowercase)
{
	ASSERT_STREQ("this is a test string.", Dive::ToLowercase("This is A Test String.").c_str());

	const std::string str("THIS IS A TEST STRING.");
	ASSERT_STREQ("this is a test string.", Dive::ToLowercase(str).c_str());
}

TEST(StringUtils, GetStringAfterToken)
{
	ASSERT_STREQ("love is great.", Dive::GetStringAfterToken("Our love is great.", "love").c_str());
}

//std::string GetStringBetweenTokens(const char* pStr, const char* pFirstToken, const char* pSecondToken);
TEST(StringUtils, GetStringBetweenTokens)
{
}

//std::string ReplaceToken(const char* pStr, const char* pToken, const char* pNewToken);
TEST(StringUtils, ReplaceToken)
{
}

// UNICODE
//std::wstring MbcsToWbcs(const std::string str, unsigned int mbcsEncoding = CP_ACP);
TEST(StringUtils, MbcsToWbcs)
{
}
//std::string WbcsToMbcs(const std::wstring str, unsigned int mbcsEncoding = CP_ACP);
TEST(StringUtils, WbcsToMbcs)
{
}
//std::string ConvertMbcs(const std::string str, unsigned int fromEncoding, unsigned int toEncoding);
TEST(StringUtils, ConvertMbcs)
{
}
//std::wstring Utf8ToUtf16(const std::string str);
TEST(StringUtils, Utf8ToUtf16)
{
}
//std::string Utf16ToUtf8(const std::wstring str);
TEST(StringUtils, Utf16ToUtf8)
{
}