#include "divepch.h"
#include "FileStream.h"
#include "Log.h"

namespace Dive
{
	FileStream::FileStream(const std::string& filepath, uint32_t flags)
		: m_bOpen(false),
		m_Flags(flags)
	{
		int iosFlags = std::ios::binary;
		iosFlags |= (flags & eFileStreamMode::Read) ? std::ios::in : 0;
		iosFlags |= (flags & eFileStreamMode::Write) ? std::ios::out : 0;
		iosFlags |= (flags & eFileStreamMode::Append) ? std::ios::app : 0;

		if (m_Flags & eFileStreamMode::Write)
		{
			m_Out.open(filepath, iosFlags);
			if (m_Out.fail())
			{
				DV_LOG_ENGINE_ERROR("파일을 쓰기모드로 여는데 실패하였습니다 - {:s}", filepath);
				return;
			}
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_In.open(filepath, iosFlags);
			if (m_In.fail())
			{
				DV_LOG_ENGINE_ERROR("파일을 읽기모드로 여는데 실패하였습니다 - {:s}", filepath);
				return;
			}
		}

		m_bOpen = true;
	}

	FileStream::~FileStream()
	{
		Close();
	}

	void FileStream::Close()
	{
		if (m_Flags & eFileStreamMode::Write)
		{
			m_Out.flush();
			m_Out.close();
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_In.clear();
			m_In.close();
		}

		m_bOpen = false;
	}

	void FileStream::Skip(uint64_t n)
	{
		if (m_Flags & eFileStreamMode::Write)
		{
			m_Out.seekp(n, std::ios::cur);
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_In.ignore(n, std::ios::cur);
		}
	}

	void FileStream::Write(const std::string& value)
	{
		auto length = static_cast<uint32_t>(value.size());
		Write(length);

		m_Out.write(value.c_str(), length);
	}

	void FileStream::Write(const std::vector<std::string>& value)
	{
		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		for (uint32_t i = 0; i <= size; i++)
		{
			Write(value[i]);
		}
	}

	void FileStream::Write(const std::vector<uint32_t>& value)
	{
		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		m_Out.write(reinterpret_cast<const char*>(&value[0]), sizeof(uint32_t) * size);
	}

	void FileStream::Write(const std::vector<unsigned char>& value)
	{
		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		m_Out.write(reinterpret_cast<const char*>(&value[0]), sizeof(unsigned char) * size);
	}

	void FileStream::Write(const std::vector<std::byte>& value)
	{
		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		m_Out.write(reinterpret_cast<const char*>(&value[0]), sizeof(std::byte) * size);
	}

	void FileStream::Read(std::string* pValue)
	{
		if (pValue == nullptr)
			return;

		// 길이
		auto length = ReadAs<uint32_t>();
		pValue->resize(length);

		// 길이만큼 읽기
		m_In.read(const_cast<char*>(pValue->c_str()), length);
	}

	void FileStream::Read(std::vector<std::string>* pValue)
	{
		if (pValue == nullptr)
			return;

		// vector size
		auto size = ReadAs<uint32_t>();

		// 요소마다 std::string 만큼 읽기
		std::string str;
		for (uint32_t i = 0; i <= size; i++)
		{
			Read(&str);
			pValue->emplace_back(str);
		}
	}

	void FileStream::Read(std::vector<uint32_t>* pValue)
	{
		if (pValue == nullptr)
			return;

		auto size = ReadAs<uint32_t>();
		pValue->resize(size);

		m_In.read(reinterpret_cast<char*>(pValue->data()), sizeof(uint32_t) * size);
	}

	void FileStream::Read(std::vector<unsigned char>* pValue)
	{
		if (pValue == nullptr)
			return;

		auto size = ReadAs<uint32_t>();
		pValue->resize(size);

		m_In.read(reinterpret_cast<char*>(pValue->data()), sizeof(unsigned char) * size);
	}

	void FileStream::Read(std::vector<std::byte>* pValue)
	{
		if (pValue == nullptr)
			return;

		auto size = ReadAs<uint32_t>();
		pValue->resize(size);

		m_In.read(reinterpret_cast<char*>(pValue->data()), sizeof(std::byte) * size);
	}
}