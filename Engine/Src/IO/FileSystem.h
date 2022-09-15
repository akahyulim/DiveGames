#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Context;

	class FileSystem : public Object
	{
		DIVE_OBJECT(FileSystem, Object)

	public:
		explicit FileSystem(Context* pContext);
		~FileSystem() override;

	private:
	private:
	};

	// 대문자로 변환.
	std::string ToUpperCase(const std::string& str);
	// 소문자로 변환.
	std::string ToLowerCase(const std::string& str);
	// 좌측 공백(혹은 특정 문자) 제거.
	std::string StringLeftTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// 우측 공백(혹은 특정 문자) 제거.
	std::string StringRightTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// 전체 공백(혹은 특정 문자) 제거.
	std::string StringTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// \\를 /로 변환.
	std::string GetInternalPath(const std::string& fullPath);
	// /를 \\로 변환.
	std::string GetNativePath(const std::string& fullPath);
	// 전체경로를 경로, 파일이름, 확장자로 나누어 전달.
	void SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension = true);
	// 전체경로 중 경로만 뽑아서 반환.
	std::string GetPath(const std::string& fullPath);
	// 전체경로 중 파일이름만 뽑아서 반환(확장자 미포함).
	std::string GetFileName(const std::string& fullPath);
	// 전체경로 중 확장자만 뽑아서 반환(.포함)
	std::string GetExtension(const std::string& fullPath, bool bLowercase = true);
	// 전체경로 중 파일이름 + 확장자만 뽑아서 반환.
	std::string GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension = false);
	// 상위 디렉토리 반환.
	std::string GetParentPath(const std::string& fullPath);
	// 절대 경로 유무 확인.
	bool IsAbsolutePath(const std::string& fullPath);
}