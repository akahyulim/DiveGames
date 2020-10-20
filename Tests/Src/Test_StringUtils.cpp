#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

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

TEST(StringUtils, GetStringBetweenTokens)
{
	ASSERT_STREQ(" Song: Plastic ", Dive::GetStringBetweenTokens("Next Song: Plastic Love.", "Next", "Love").c_str());
}

TEST(StringUtils, ReplaceToken)
{
	ASSERT_STREQ("Adobe Imageshop CC", Dive::ReplaceToken("Adobe Photoshop CC", "Photo", "Image").c_str());
}