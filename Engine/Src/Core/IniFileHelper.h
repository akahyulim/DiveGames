#pragma once

namespace Dive
{
	namespace IniFileHelper
	{
		//= Get
		int GetInt(const std::wstring& section, const std::wstring& key, const std::wstring& filename);
		float GetFloat(const std::wstring& section, const std::wstring& key, const std::wstring& filename);
		bool GetBool(const std::wstring& section, const std::wstring& key, const std::wstring& filename);
		std::wstring GetStr(const std::wstring& section, const std::wstring& key, const std::wstring& filename);

		//= Set
		void SetInit(const std::wstring& section, const std::wstring& key, int value, const std::wstring& filename);
		void SetFloat(const std::wstring& section, const std::wstring& key, float value, const std::wstring& filename);
		void SetBool(const std::wstring& section, const std::wstring& key, bool value, const std::wstring& filename);
		void SetStr(const std::wstring& section, const std::wstring& key, const std::wstring& value, const std::wstring& filename);
	}
}