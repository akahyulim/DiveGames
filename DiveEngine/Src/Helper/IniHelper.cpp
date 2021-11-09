#include "IniHelper.h"

namespace DiveEngine
{
	Key::Key(const std::string& fileName, const std::string& sectionName, const std::string& keyName)
		: m_FileName(fileName), m_SectionName(sectionName), m_KeyName(keyName)
	{
		m_DefaultStringValue = "";
	}

	Key::operator int()
	{
		auto value = getKeyValue(m_DefaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator unsigned int()
	{
		auto value = getKeyValue(m_DefaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator bool()
	{
		auto value = getKeyValue(m_bDefaultBoolValue, "%d");
		return std::stoi(value.c_str()) == 0 ? false : true;
	}

	Key::operator double()
	{
		auto value = getKeyValue(m_DefaultDoubleValue, "%.4f");
		return std::stod(value.c_str());
	}

	Key::operator std::string()
	{
		auto value = getKeyValue(m_DefaultStringValue.c_str(), "%s");
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

	Key& Key::operator=(const char* pValue)
	{
		setKeyValue(pValue, "%s");
		return *this;
	}

	Key & Key::operator=(std::string value)
	{
		setKeyValue(value.c_str(), "%s");
		return *this;
	}

	Key& Key::operator<<(int value)
	{
		m_DefaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(unsigned int value)
	{
		m_DefaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(bool value)
	{
		m_bDefaultBoolValue = value;
		return *this;
	}

	Key& Key::operator<<(double value)
	{
		m_DefaultDoubleValue = value;
		return *this;
	}

	Key& Key::operator<<(const char* pValue)
	{
		m_DefaultStringValue = pValue;
		return *this;
	}

	Key& Key::operator<<(std::string value)
	{
		m_DefaultStringValue = std::move(value);
		return *this;
	}

	Section::Section(const std::string& fileName, const std::string& sectionName)
		: m_FileName(fileName), m_SectionName(sectionName)
	{}

	Key Section::operator[](const std::string& keyName)
	{
		return Key(m_FileName, m_SectionName, keyName);
	}

	IniHelper::IniHelper(const std::string fileName)
		: m_FileName(fileName)
	{
	}

	Section IniHelper::operator[](const std::string& sectionName)
	{
		return Section(m_FileName, sectionName);
	}
}