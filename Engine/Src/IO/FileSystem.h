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

	// �빮�ڷ� ��ȯ.
	std::string ToUpperCase(const std::string& str);
	// �ҹ��ڷ� ��ȯ.
	std::string ToLowerCase(const std::string& str);
	// ���� ����(Ȥ�� Ư�� ����) ����.
	std::string StringLeftTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// ���� ����(Ȥ�� Ư�� ����) ����.
	std::string StringRightTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// ��ü ����(Ȥ�� Ư�� ����) ����.
	std::string StringTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	// \\�� /�� ��ȯ.
	std::string GetInternalPath(const std::string& fullPath);
	// /�� \\�� ��ȯ.
	std::string GetNativePath(const std::string& fullPath);
	// ��ü��θ� ���, �����̸�, Ȯ���ڷ� ������ ����.
	void SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension = true);
	// ��ü��� �� ��θ� �̾Ƽ� ��ȯ.
	std::string GetPath(const std::string& fullPath);
	// ��ü��� �� �����̸��� �̾Ƽ� ��ȯ(Ȯ���� ������).
	std::string GetFileName(const std::string& fullPath);
	// ��ü��� �� Ȯ���ڸ� �̾Ƽ� ��ȯ(.����)
	std::string GetExtension(const std::string& fullPath, bool bLowercase = true);
	// ��ü��� �� �����̸� + Ȯ���ڸ� �̾Ƽ� ��ȯ.
	std::string GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension = false);
	// ���� ���丮 ��ȯ.
	std::string GetParentPath(const std::string& fullPath);
	// ���� ��� ���� Ȯ��.
	bool IsAbsolutePath(const std::string& fullPath);
}