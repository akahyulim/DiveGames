#include "divepch.h"
#include "FileStream.h"
#include "Base/Base.h"

namespace Dive
{
	FileStream::FileStream(const std::string& filepath, uint32_t flags)
	{
		m_bOpen = false;
		m_Flags = flags;

		int iosFlags = std::ios::binary;
		iosFlags |= (flags & eFileStreamMode::Read)		? std::ios::in	: 0;
		iosFlags |= (flags & eFileStreamMode::Write)	? std::ios::out : 0;
		iosFlags |= (flags & eFileStreamMode::Append)	? std::ios::app : 0;

		if (m_Flags & eFileStreamMode::Write)
		{
			m_fOut.open(filepath, iosFlags);
			if(m_fOut.fail())
			{
				DV_CORE_ERROR("파일을 쓰기모드로 여는데 실패하였습니다 - {:s}", filepath);
				return;
			}
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_fIn.open(filepath, iosFlags);
			if (m_fIn.fail())
			{
				DV_CORE_ERROR("파일을 읽기모드로 여는데 실패하였습니다 - {:s}", filepath);
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
			m_fOut.flush();
			m_fOut.close();
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_fIn.clear();
			m_fIn.close();
		}
	}
	
	void FileStream::Write(const std::string& value)
	{
		auto length = static_cast<uint32_t>(value.size());
		Write(length);

		m_fOut.write(const_cast<char*>(value.c_str()), length);
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
		auto length = static_cast<uint32_t>(value.size());
		Write(length);

		m_fOut.write(reinterpret_cast<const char*>(&value[0]), sizeof(uint32_t) * length);
	}

	void FileStream::Write(const std::vector<unsigned char>& value)
	{
		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		m_fOut.write(reinterpret_cast<const char*>(&value[0]), sizeof(unsigned char) * size);
	}

	void FileStream::Write(const std::vector<std::byte>& value)
	{

		auto size = static_cast<uint32_t>(value.size());
		Write(size);

		m_fOut.write(reinterpret_cast<const char*>(&value[0]), sizeof(std::byte) * size);
	}

	void FileStream::Skip(uint64_t n)
	{
		if (m_Flags & eFileStreamMode::Write)
		{
			m_fOut.seekp(n, std::ios::cur);
		}
		else if (m_Flags & eFileStreamMode::Read)
		{
			m_fIn.ignore(n, std::ios::cur);
		}
	}

	void FileStream::Read(std::string* pValue)
	{
		if (pValue == nullptr)
			return;

		uint32_t length = 0;
		Read(&length);

		pValue->resize(length);
		
		m_fIn.read(const_cast<char*>(pValue->c_str()), length);
	}
	
	void FileStream::Read(std::vector<std::string>* pValue)
	{
		if (pValue == nullptr)
			return;

		pValue->clear();
		pValue->shrink_to_fit();

		uint32_t size = 0;
		Read(&size);

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

		pValue->clear();
		pValue->shrink_to_fit();

		// 얘부터는 왜 이렇게 하냐...?!
		auto length = ReadAs<uint32_t>();

		// 공간 확보 및 초기화...? 일단 이것들부터 확실히 하자.
		pValue->reserve(length);
		pValue->resize(length);

		m_fIn.read(reinterpret_cast<char*>(pValue->data()), sizeof(uint32_t) * length);
	}

	void FileStream::Read(std::vector<unsigned char>* pValue)
	{
		if (pValue == nullptr)
			return;

		pValue->clear();
		pValue->shrink_to_fit();
	}
	
	void FileStream::Read(std::vector<std::byte>* pValue)
	{
		if (pValue == nullptr)
			return;

		pValue->clear();
		pValue->shrink_to_fit();
	}
}