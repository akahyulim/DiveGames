#include "DivePch.h"
#include "IniHelper.h"

namespace Dive
{
	Key::Key(const std::string& fileName, const std::string& sectionName, const std::string& keyName)
		: m_fileName(fileName), m_sectionName(sectionName), m_keyName(keyName)
	{
		m_defaultStrValue = "";
	}

	Key::operator int()
	{
		auto value = getKeyValue(m_defaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator unsigned int()
	{
		auto value = getKeyValue(m_defaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator bool()
	{
		auto value = getKeyValue(m_bDefaultBoolValue, "%d");
		return std::stoi(value.c_str()) == 0 ? false : true;
	}

	Key::operator double()
	{
		auto value = getKeyValue(m_defaultDoubleValue, "%.4f");
		return std::stod(value.c_str());
	}

	Key::operator std::string()
	{
		auto value = getKeyValue(m_defaultStrValue.c_str(), "%s");
		return value;
	}

	Key& Key::operator=(int value)
	{
		setKeyValue(value, "%d");
		return *this;
	}

	Key& Key::operator=(unsigned int value)
	{
		setKeyValue(value, "%d");
		return *this;
	}

	Key& Key::operator=(bool value)
	{
		setKeyValue(value, "%d");
		return *this;
	}

	Key& Key::operator=(double value)
	{
		setKeyValue(value, "%.4f");
		return *this;
	}

	Key& Key::operator=(const char* value)
	{
		setKeyValue(value, "%s");
		return *this;
	}

	Key & Key::operator=(std::string value)
	{
		setKeyValue(value.c_str(), "%s");
		return *this;
	}

	Key& Key::operator<<(int value)
	{
		m_defaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(unsigned int value)
	{
		m_defaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(bool value)
	{
		m_bDefaultBoolValue = value;
		return *this;
	}

	Key& Key::operator<<(double value)
	{
		m_defaultDoubleValue = value;
		return *this;
	}

	Key& Key::operator<<(const char* value)
	{
		m_defaultStrValue = value;
		return *this;
	}

	Key& Key::operator<<(std::string value)
	{
		m_defaultStrValue = std::move(value);
		return *this;
	}

	Section::Section(const std::string& fileName, const std::string& sectionName)
		: m_fileName(fileName), m_sectionName(sectionName)
	{}

	Key Section::operator[](const std::string& keyName)
	{
		return Key(m_fileName, m_sectionName, keyName);
	}

	IniHelper::IniHelper(const std::string fileName)
		: m_fileName(std::move(fileName))
	{
	}

	Section IniHelper::operator[](const std::string& sectionName)
	{
		return Section(m_fileName, sectionName);
	}
}