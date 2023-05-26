#pragma once

namespace Dive
{
	void ErrorDialog(const std::string& title, const std::string& message);
	void ErrorDialog(const std::wstring& title, const std::wstring& message);
	void ErrorExit(const std::string& message, int exitCode = EXIT_FAILURE);
	void ErrorExit(const std::wstring& message, int exitCode = EXIT_FAILURE);

	// 굳이 타입별로 다 만들 필요가 없을 것 같다.
	// GetCommnadLine()을 쓰면 wchar 배열로 리턴하는 듯 하다.
	std::vector<std::string> ParseArguments(const char* pCmdLine);
	std::vector<std::string> ParseArguments(const wchar_t* pCmdLine);
	std::vector<std::string> ParseArguments(const std::string& pCmdLine);
	std::vector<std::string> ParseArguments(const std::wstring& pCmdLine);
	std::vector<std::string> ParseArguments(int argc, char** argv);
	std::vector<std::string> GetArguments();
}