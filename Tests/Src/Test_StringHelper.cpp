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
