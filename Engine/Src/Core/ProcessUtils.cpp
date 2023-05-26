#include "DivePch.h"
#include "ProcessUtils.h"
#include "IO/FileSystem.h"

namespace Dive
{
	static std::vector<std::string> s_Arguments;

	void ErrorDialog(const std::string& title, const std::string& message)
	{
		MessageBox(nullptr, FileSystem::StringToWstring(message).c_str(), FileSystem::StringToWstring(title).c_str(), MB_OK);
	}

	void ErrorDialog(const std::wstring& title, const std::wstring& message)
	{
		MessageBox(nullptr, message.c_str(), title.c_str(), MB_OK);
	}

	void ErrorExit(const std::string& message, int exitCode)
	{
		if (!message.empty())
			std::cout << message.c_str() << std::endl;

		exit(exitCode);
	}

	void ErrorExit(const std::wstring& message, int exitCode)
	{
		if (!message.empty())
			std::wcout << message.c_str() << std::endl;

		exit(exitCode);
	}
	
	std::vector<std::string> ParseArguments(const char* pCmdLine)
	{
		return std::vector<std::string>();
	}
	
	std::vector<std::string> ParseArguments(const wchar_t* pCmdLine)
	{
		return std::vector<std::string>();
	}
	
	std::vector<std::string> ParseArguments(const std::string& pCmdLine)
	{
		return std::vector<std::string>();
	}
	
	std::vector<std::string> ParseArguments(const std::wstring& pCmdLine)
	{
		return std::vector<std::string>();
	}
	
	std::vector<std::string> ParseArguments(int argc, char** argv)
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> GetArguments()
	{
		return s_Arguments;
	}
}