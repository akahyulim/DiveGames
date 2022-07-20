#pragma once
#include <xhash>
#include <string>

namespace Dive
{
	class StringHash
	{
	public:
		StringHash() : m_Value(0) {}
		StringHash(const std::string& str) : m_Value(Calcurate(str.c_str())) {}
		StringHash(const char* pStr) : m_Value(Calcurate(pStr)) {}
		StringHash(unsigned int value) : m_Value(value) {}
		~StringHash() = default;

		bool operator==(const StringHash& rhs) const { return m_Value == rhs.Value(); }
		bool operator!=(const StringHash& rhs) const { return m_Value != rhs.Value(); }
		bool operator<(const StringHash& rhs) const { return m_Value < rhs.Value(); }
		bool operator>(const StringHash& rhs) const { return m_Value > rhs.Value(); }
		explicit operator bool() const { return m_Value != 0; }

		static unsigned int Calcurate(const std::string& str, unsigned int value = 0)
		{
			if (str.empty())
				return value;

			static std::hash<std::string> strHash;
			return static_cast<unsigned int>(strHash(str));
		}

		unsigned int Value() const { return m_Value; }
		std::string ValueStr() const { return std::to_string(m_Value); }

	private:
		unsigned int m_Value;
	};
}