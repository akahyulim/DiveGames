#pragma once
#include <xhash>
#include <string>

namespace Dive
{
	// 문자열을 해시로 변환하여 관리하는 클래스.
	class StringHash
	{
	public:
		StringHash() : m_Value(0) {}
		StringHash(const std::string& str) : m_Value(Calcurate(str.c_str())) {}
		StringHash(const char* pStr) : m_Value(Calcurate(pStr)) {}
		StringHash(uint32_t value) : m_Value(value) {}
		~StringHash() = default;

		bool operator==(const StringHash& rhs) const { return m_Value == rhs.GetValue(); }
		bool operator!=(const StringHash& rhs) const { return m_Value != rhs.GetValue(); }
		bool operator<(const StringHash& rhs) const { return m_Value < rhs.GetValue(); }
		bool operator>(const StringHash& rhs) const { return m_Value > rhs.GetValue(); }
		explicit operator bool() const { return m_Value != 0; }

		static uint32_t Calcurate(const std::string& str, uint32_t value = 0)
		{
			if (str.empty())
				return value;

			static std::hash<std::string> strHash;
			return static_cast<uint32_t>(strHash(str));
		}

		uint32_t GetValue() const { return m_Value; }
		std::string GetValueToStr() const { return std::to_string(m_Value); }

	private:
		uint32_t m_Value;
	};
}