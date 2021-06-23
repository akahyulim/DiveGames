#include "IniHelper.h"

namespace dive
{
	Key::Key(const std::string& fileName, const std::string& sectionName, const std::string& keyName)
		: mFileName(fileName), mSectionName(sectionName), mKeyName(keyName)
	{
		mDefaultStringValue = "";
	}

	Key::operator int()
	{
		auto value = getKeyValue(mDefaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator unsigned int()
	{
		auto value = getKeyValue(mDefaultIntValue, "%d");
		return std::stoi(value.c_str());
	}

	Key::operator bool()
	{
		auto value = getKeyValue(mbDefaultBoolValue, "%d");
		return std::stoi(value.c_str()) == 0 ? false : true;
	}

	Key::operator double()
	{
		auto value = getKeyValue(mDefaultDoubleValue, "%.4f");
		return std::stod(value.c_str());
	}

	Key::operator std::string()
	{
		auto value = getKeyValue(mDefaultStringValue.c_str(), "%s");
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
		mDefaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(unsigned int value)
	{
		mDefaultIntValue = value;
		return *this;
	}

	Key& Key::operator<<(bool value)
	{
		mbDefaultBoolValue = value;
		return *this;
	}

	Key& Key::operator<<(double value)
	{
		mDefaultDoubleValue = value;
		return *this;
	}

	Key& Key::operator<<(const char* value)
	{
		mDefaultStringValue = value;
		return *this;
	}

	Key& Key::operator<<(std::string value)
	{
		mDefaultStringValue = std::move(value);
		return *this;
	}

	Section::Section(const std::string& fileName, const std::string& sectionName)
		: mFileName(fileName), mSectionName(sectionName)
	{}

	Key Section::operator[](const std::string& keyName)
	{
		return Key(mFileName, mSectionName, keyName);
	}

	IniHelper::IniHelper(const std::string fileName)
		: mFileName(fileName)
	{
	}

	Section IniHelper::operator[](const std::string& sectionName)
	{
		return Section(mFileName, sectionName);
	}
}