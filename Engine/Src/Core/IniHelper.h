#pragma once

namespace Dive
{
	constexpr size_t MAX_VALUE_SIZE = 256;

	class Key
	{
	public:
		Key(const std::string& fileName, const std::string& sectionName, const std::string& keyName);
		~Key() = default;

		operator int();
		operator bool();
		operator double();
		operator std::string();

		Key& operator=(int value);
		Key& operator=(bool value);
		Key& operator=(double value);
		Key& operator=(const char* value);
		Key& operator=(std::string value);

		Key& operator<<(int value);
		Key& operator<<(bool value);
		Key& operator<<(double value);
		Key& operator<<(const char* value);
		Key& operator<<(std::string value);

	private:
		template<typename T>
		std::string getKeyValue(const T& defaultValue, const char* formatSpec);
		template<typename T>
		void setKeyValue(T value, const char* formatSpec);

	private:
		std::string m_FileName;
		std::string m_SectionName;
		std::string m_KeyName;

		std::string m_DefaultStrValue;
		union
		{
			int m_DefaultIntValue;
			bool m_bDefaultBoolValue;
			double m_DefaultDoubleValue;
		};
	};

	template<typename T>
	std::string Key::getKeyValue(const T& defaultValue, const char* formatSpec)
	{
		char defaultStr[MAX_VALUE_SIZE];
		sprintf_s(defaultStr, MAX_VALUE_SIZE, formatSpec, defaultValue);
		std::unique_ptr<char> pBuffer(new char[MAX_VALUE_SIZE]);
		GetPrivateProfileStringA(m_SectionName.c_str(), m_KeyName.c_str(), defaultStr, pBuffer.get(), MAX_VALUE_SIZE, m_FileName.c_str());
		
		return std::string(pBuffer.get());
	}

	template<typename T>
	void Key::setKeyValue(T value, const char* formatSpec)
	{
		char valueStr[MAX_VALUE_SIZE];
		sprintf_s(valueStr, MAX_VALUE_SIZE, formatSpec, value);
		WritePrivateProfileStringA(m_SectionName.c_str(), m_KeyName.c_str(), valueStr, m_FileName.c_str());
	}

	class Section
	{
	public:
		Section(const std::string& fileName, const std::string& sectionName);
		~Section() = default;

		Key operator[](const std::string& keyName);

	private:
		std::string m_FileName;
		std::string m_SectionName;
	};

	class IniHelper
	{
	public:
		IniHelper(const std::string fileName);
		~IniHelper() = default;

		Section operator[](const std::string& sectionName);

	private:
		std::string m_FileName;
	};
}

