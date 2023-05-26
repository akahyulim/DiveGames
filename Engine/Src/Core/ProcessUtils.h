#pragma once

namespace Dive
{
	void ErrorDialog(const std::string& title, const std::string& message);
	void ErrorDialog(const std::wstring& title, const std::wstring& message);
	void ErrorExit(const std::string& message, int exitCode = EXIT_FAILURE);
	void ErrorExit(const std::wstring& message, int exitCode = EXIT_FAILURE);

	// ���� Ÿ�Ժ��� �� ���� �ʿ䰡 ���� �� ����.
	// GetCommnadLine()�� ���� wchar �迭�� �����ϴ� �� �ϴ�.
	std::vector<std::string> ParseArguments(const char* pCmdLine);
	std::vector<std::string> ParseArguments(const wchar_t* pCmdLine);
	std::vector<std::string> ParseArguments(const std::string& pCmdLine);
	std::vector<std::string> ParseArguments(const std::wstring& pCmdLine);
	std::vector<std::string> ParseArguments(int argc, char** argv);
	std::vector<std::string> GetArguments();
}