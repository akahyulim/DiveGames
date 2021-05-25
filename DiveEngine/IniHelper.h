#pragma once
#include <string>
#include <memory>

namespace Dive
{
	constexpr size_t MAX_VALUE_SIZE = 256;

	class Key
	{
	public:
		Key(const std::string& fileName, const std::string& sectionName, const std::string& keyName);
		~Key() = default;

		operator int();
		operator unsigned int();
		operator bool();
		operator double();
		operator std::string();

		Key& operator=(int value);
		Key& operator=(unsigned int value);
		Key& operator=(bool value);
		Key& operator=(double value);
		Key& operator=(const char* value);
		Key& operator=(std::string value);

		//======================================================================//
		//	대상 Key에 값이 존재하지 않을 경우에만 전달받은 값을 리턴합니다.	//
		//	전달받은 값으로 초기화하지 않습니다.								//
		//======================================================================//
		Key& operator<<(int value);
		Key& operator<<(unsigned int value);
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
		std::string m_fileName;
		std::string m_sectionName;
		std::string m_keyName;

		std::string m_defaultStrValue;
		union
		{
			int m_defaultIntValue;
			unsigned int m_defaultUintValue;
			bool m_bDefaultBoolValue;
			double m_defaultDoubleValue;
		};
	};

	template<typename T>
	std::string Key::getKeyValue(const T& defaultValue, const char* formatSpec)
	{
		char defaultStr[MAX_VALUE_SIZE];
		sprintf_s(defaultStr, MAX_VALUE_SIZE, formatSpec, defaultValue);
		std::unique_ptr<char> pBuffer(new char[MAX_VALUE_SIZE]);
		//GetPrivateProfileStringA(m_sectionName.c_str(), m_keyName.c_str(), defaultStr, pBuffer.get(), MAX_VALUE_SIZE, m_fileName.c_str());
		
		return std::string(pBuffer.get());
	}

	template<typename T>
	void Key::setKeyValue(T value, const char* formatSpec)
	{
		char valueStr[MAX_VALUE_SIZE];
		sprintf_s(valueStr, MAX_VALUE_SIZE, formatSpec, value);
		//WritePrivateProfileStringA(m_sectionName.c_str(), m_keyName.c_str(), valueStr, m_fileName.c_str());
	}

	class Section
	{
	public:
		Section(const std::string& fileName, const std::string& sectionName);
		~Section() = default;

		Key operator[](const std::string& keyName);

	private:
		std::string m_fileName;
		std::string m_sectionName;
	};

	class IniHelper
	{
	public:
		IniHelper(const std::string fileName);
		~IniHelper() = default;

		Section operator[](const std::string& sectionName);

	private:
		std::string m_fileName;
	};
}

