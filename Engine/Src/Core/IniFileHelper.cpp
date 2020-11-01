#include "DivePch.h"
#include "IniFileHelper.h"
#include "FileSystemHelper.h"
#include "Log.h"

namespace Dive
{
	namespace IniFileHelper
	{
		int GetInt(const std::wstring & section, const std::wstring & key, const std::wstring & filename)
		{
			if (!FileSystemHelper::FileExists(filename))
			{
				CORE_WARN("IniFileHelper::GetInt>> No exists file.");
				return 0;
			}

			return GetPrivateProfileIntW(section.c_str(), key.c_str(), 0, filename.c_str());
		}

		float GetFloat(const std::wstring & section, const std::wstring & key, const std::wstring & filename)
		{
			float defaultValue = 0.0f;
			if (!FileSystemHelper::FileExists(filename))
			{
				CORE_WARN("IniFileHelper::GetInt>> No exists file.");
				return defaultValue;
			}

			wchar_t value[16]{ 0, };
			GetPrivateProfileStringW(section.c_str(), key.c_str(), value, value, 16, filename.c_str());

			return _wtof(value);
		}

		bool GetBool(const std::wstring & section, const std::wstring & key, const std::wstring & filename)
		{
			bool bDefault = false;
			if (!FileSystemHelper::FileExists(filename))
			{
				CORE_WARN("IniFileHelper::GetInt>> No exists file.");
				return bDefault;
			}

			wchar_t value[8]{ L"false" };
			GetPrivateProfileStringW(section.c_str(), key.c_str(), value, value, 8, filename.c_str());

			if (wcscmp(value, L"TRUE") == 0 || wcscmp(value, L"True") == 0 || wcscmp(value, L"true") == 0)
				return true;

			return false;
		}
		
		std::wstring GetStr(const std::wstring & section, const std::wstring & key, const std::wstring & filename)
		{
			std::wstring defaultValue;
			if (!FileSystemHelper::FileExists(filename))
			{
				CORE_WARN("IniFileHelper::GetInt>> No exists file.");
				return defaultValue;
			}

			wchar_t value[MAX_PATH]{ 0, };
			GetPrivateProfileStringW(section.c_str(), key.c_str(), value, value, MAX_PATH, filename.c_str());

			return value;
		}

		void SetInit(const std::wstring & section, const std::wstring & key, int value, const std::wstring & filename)
		{
			WritePrivateProfileStringW(section.c_str(), key.c_str(), std::to_wstring(value).c_str(), filename.c_str());
		}

		void SetFloat(const std::wstring & section, const std::wstring & key, float value, const std::wstring & filename)
		{
			WritePrivateProfileStringW(section.c_str(), key.c_str(), std::to_wstring(value).c_str(), filename.c_str());
		}

		void SetBool(const std::wstring & section, const std::wstring & key, bool value, const std::wstring & filename)
		{
			WritePrivateProfileStringW(section.c_str(), key.c_str(), value ? L"true" : L"false", filename.c_str());
		}

		void SetStr(const std::wstring & section, const std::wstring & key, const std::wstring & value, const std::wstring & filename)
		{
			WritePrivateProfileStringW(section.c_str(), key.c_str(), value.c_str(), filename.c_str());
		}
	}
}