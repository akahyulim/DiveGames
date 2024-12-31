#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define DV_LOG(CategoryName, Level, Format, ...) \
	do { \
		auto pLogger = Dive::LogManager::GetLogger(#CategoryName); \
		pLogger->log(spdlog::level::Level, Format, ##__VA_ARGS__); \
	} while (0)

namespace Dive
{
	class LogManager
	{
	public:
		static std::shared_ptr<spdlog::logger> GetLogger(const std::string& category);
		static void Shutdown() { spdlog::shutdown(); }

		// �ݵ�� �ΰ� ��� ���� �����ؾ� �Ѵ�.
		static void SetLevels(spdlog::level::level_enum setLevel, spdlog::level::level_enum flushLevel) { s_SetLevel = setLevel; s_FlushLevel = flushLevel; }
		static void SetFilename(const std::string& filename) { s_Filename = filename; }
		
		// ��� Ȯ�ο�(���� �Ұ�)
		static const std::string& GetFilename() { return s_Filename; }

	private:
		static std::string s_Filename;
		static spdlog::level::level_enum s_SetLevel;
		static spdlog::level::level_enum s_FlushLevel;
	};
}