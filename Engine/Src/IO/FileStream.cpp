#include "divepch.h"
#include "FileStream.h"
#include "FileSystem.h"
#include "Log.h"

namespace Dive
{
	FileStream::FileStream(const std::string& path, uint32_t flags)
		: m_Flags(flags),
		m_bOpen(false),
		m_Size(0)
	{
		std::ios_base::openmode iosFlags = std::ios::binary;
		if (m_Flags & eFileStreamModeFlags::Read) iosFlags |= std::ios::in;
		if (m_Flags & eFileStreamModeFlags::Write) iosFlags |= std::ios::out;
		if (m_Flags & eFileStreamModeFlags::Append) iosFlags |= std::ios::app;

		if (m_Flags & eFileStreamModeFlags::Write)
		{
			m_Out.open(path, iosFlags);
			if (m_Out.fail())
			{
				DV_LOG_ENGINE_ERROR("FileStream::FileStream - ����({:s})�� ������� ���µ� �����Ͽ����ϴ�.", path);
				return;
			}
		}
		else if (m_Flags & eFileStreamModeFlags::Read)
		{
			m_In.open(path, iosFlags);
			if (m_In.fail())
			{
				DV_LOG_ENGINE_ERROR("FileStream::FileStream - ����({:s})�� �б���� ���µ� �����Ͽ����ϴ�.", path);
				return;
			}

			// ũ�� ���
			m_In.seekg(0, std::ios::end);
			m_Size = static_cast<unsigned int>(m_In.tellg());
			m_In.seekg(0, std::ios::beg);
		}

		m_Filepath = path;
		m_Filename = FileSystem::GetFileNameAndExtension(path);

		m_bOpen = true;
	}

	FileStream::~FileStream()
	{
		Close();

		DV_LOG_ENGINE_TRACE("FileStream �Ҹ� �Ϸ�({:s})", m_Filename);
	}

	void FileStream::Close()
	{
		if (m_Flags & eFileStreamModeFlags::Write)
		{
			m_Out.flush();
			m_Out.close();
		}
		else if (m_Flags & eFileStreamModeFlags::Read)
		{
			m_In.clear();
			m_In.close();
		}

		m_bOpen = false;
	}

	void FileStream::Skip(uint64_t n)
	{
		if (m_Flags & eFileStreamModeFlags::Write)
		{
			m_Out.seekp(n, std::ios::cur);
		}
		else if (m_Flags & eFileStreamModeFlags::Read)
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

	// ����, �ε����� ��� ũ�Ⱑ �ٸ� �� �ִ�.
	// ���� void*�� �����ϰ�, size * count�� ���޹޾� ���� / �б⸦ �ϴ� ���� ���ϴ�.
	void FileStream::Write(const void* pData, unsigned int size)
	{
		if (!size)
			return;

		m_Out.write(reinterpret_cast<const char*>(pData), size);
	}

	void FileStream::Read(std::string* pValue)
	{
		if (!m_bOpen || pValue == nullptr)
			return;
		
		// ���̰� ���� ����Ǿ� �־�� �Ѵ�.
		uint32_t length = 0;
		Read(&length);

		pValue->resize(length);
		m_In.read(const_cast<char*>(pValue->c_str()), length);
	}

	void FileStream::Read(std::vector<std::string>* pValue)
	{
		if (pValue == nullptr)
			return;

		// vector size
		auto size = ReadAs<uint32_t>();

		// ��Ҹ��� std::string ��ŭ �б�
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

	void FileStream::Read(void* pData, unsigned int size)
	{
		if (!size)
			return;

		m_In.read(reinterpret_cast<char*>(pData), size);
	}

	std::string FileStream::ReadLine()
	{
		std::string lineStr;
		std::getline(m_In, lineStr);

		return lineStr;
	}

	bool FileStream::IsEof()
	{
		return static_cast<unsigned int>(m_In.tellg()) >= m_Size;
	}
}