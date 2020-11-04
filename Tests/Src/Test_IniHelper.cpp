#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

using namespace Dive::FileSystemHelper;
using namespace Dive;

TEST(IniHelper, SetGetInt)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());
	
	auto section = helper["Display"];
	section["nWidth"] = 1600;
	section["nHeight"] = 900;
	int width = section["nWidth"];
	int height = section["nHeight"];

	EXPECT_EQ(1600, width);
	EXPECT_EQ(900, height);

	section["nWidth"] = -900;
	section["nHeight"] = -600;
	width = section["nWidth"];
	height = section["nHeight"];

	EXPECT_EQ(-900, width);
	EXPECT_EQ(-600, height);
}

TEST(IniHelper, SetGetBool)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());

	helper["Display"]["bVSync"] = false;
	bool bVSync = helper["Display"]["bVSync"];
	EXPECT_FALSE(bVSync);

	helper["Display"]["bVSync"] = true;
	bVSync = helper["Display"]["bVSync"];
	EXPECT_TRUE(bVSync);
}

TEST(IniHelper, SetGetDouble)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());

	auto section = helper["Display"];
	section["dNear"] = 0.5;
	section["dFar"] = 1000.0;
	double nearPlane = section["dNear"];
	double farPlane = section["dFar"];
	
	EXPECT_DOUBLE_EQ(0.5, nearPlane);
	EXPECT_DOUBLE_EQ(1000.0, farPlane);

	section["dNear"] = 0.0001;
	section["dFar"] = 500.0;
	nearPlane = section["dNear"];
	farPlane = section["dFar"];

	EXPECT_DOUBLE_EQ(0.0001, nearPlane);
	EXPECT_DOUBLE_EQ(500.0, farPlane);
}

TEST(IniHelper, SetGetString)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());

	auto section = helper["Display"];
	section["D3dDevice"] = std::string("Geforce RTX 3080");
	std::string vga = section["D3dDevice"];

	EXPECT_STREQ("Geforce RTX 3080", vga.c_str());
}

TEST(IniHelper, SetGetKorean)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());

	auto section = helper["SCORE"];
	section["1등"] = "이젠알아";
	std::string ranker = section["1등"];

	EXPECT_STREQ("이젠알아", ranker.c_str());
}

//================================================================================//
// 파일, 섹션, 키 모두 존재한다면												  // 
// 저장된 값을 얻어옵니다.														  //
// 전달한 값으로 파일 정보를 초기화하는 것이 아닙니다.							  //
//================================================================================//
TEST(IniHelper, GetReadData)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	Dive::IniHelper helper(file.c_str());

	int width = helper["Display"]["nWidth"] << 1980;
	int height = helper["Display"]["nHeight"] << 1080;
	bool vsync = helper["Display"]["bVSync"] << true;
	double nearPlane = helper["Display"]["dNear"] << 0.1;
	double farPlane = helper["Display"]["dFar"] << 1000.0;
	std::string vga = helper["Display"]["D3dDevice"] << "Geforce GTX 970";
	std::string ranker = helper["SCORE"]["1등"] << "피안화";

	EXPECT_EQ(-900, width);
	EXPECT_EQ(-600, height);
	EXPECT_TRUE(vsync);
	EXPECT_DOUBLE_EQ(0.0001, nearPlane);
	EXPECT_DOUBLE_EQ(500.0, farPlane);
	EXPECT_STREQ("Geforce RTX 3080", vga.c_str());
	EXPECT_STREQ("이젠알아", ranker.c_str());
}

//================================================================================//
// 파일, 섹션, 키 중 하나라도 존재하지 않는다면									  //
// 설정한 초기값을 리턴합니다.													  //
// 전달한 값으로 파일 정보를 초기화하는 것이 아닙니다.							  //
//================================================================================//
TEST(IniHelper, SetInitailzeData)
{
	auto file = GetWorkingDirectoryA() + "data.ini";
	EXPECT_TRUE(FileSystemHelper::DeleteFileA(file.c_str()));
	
	Dive::IniHelper helper(file.c_str());

	int width = helper["Display"]["nWidth"] << 1980;
	int height = helper["Display"]["nHeight"] << 1080;
	bool vsync = helper["Display"]["bVSync"] << true;
	double nearPlane = helper["Display"]["dNear"] << 0.1;
	double farPlane = helper["Display"]["dFar"] << 1000.0;
	std::string vga = helper["Display"]["D3dDevice"] << "Geforce GTX 970";
	std::string ranker = helper["SCORE"]["1등"] << "피안화";

	EXPECT_EQ(1980, width);
	EXPECT_EQ(1080, height);
	EXPECT_TRUE(vsync);
	EXPECT_DOUBLE_EQ(0.1, nearPlane);
	EXPECT_DOUBLE_EQ(1000.0, farPlane);
	EXPECT_STREQ("Geforce GTX 970", vga.c_str());
	EXPECT_STREQ("피안화", ranker.c_str());
}