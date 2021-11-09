#include "FileStream.h"
#include "Log.h"


namespace DiveEngine
{
	FileStream::FileStream(const std::string & filepath, eFileStreamMode mode)
		: m_bFileOpened(false),
		m_StreamMode(mode)
	{
		int iosFlags = std::ios::binary;
		iosFlags |= m_StreamMode == eFileStreamMode::Read ? std::ios::in : 0;
		iosFlags |= m_StreamMode == eFileStreamMode::Write ? std::ios::out : 0;
		iosFlags |= m_StreamMode == eFileStreamMode::Append ? std::ios::app : 0;

		if (m_StreamMode == eFileStreamMode::Write || m_StreamMode == eFileStreamMode::Append)
		{
			m_OutputStream.open(filepath, iosFlags);
			if (m_OutputStream.fail())
			{
				CORE_ERROR("");
				return;
			}
		}
		else if (m_StreamMode == eFileStreamMode::Read)
		{
			m_InputStream.open(filepath, iosFlags);
			if (m_InputStream.fail())
			{
				CORE_ERROR("");
				return;
			}
		}

		m_bFileOpened = true;
	}

	FileStream::~FileStream()
	{
		Close();
	}

	void FileStream::Close()
	{
		if (m_StreamMode == eFileStreamMode::Write || m_StreamMode == eFileStreamMode::Append)
		{
			m_OutputStream.flush();
			m_OutputStream.close();
		}
		else if (m_StreamMode == eFileStreamMode::Read)
		{
			m_InputStream.clear();
			m_InputStream.close();
		}
	}

	// 1. capacity가 0으로 초기화되지 않고 있다.
	// 2. value->c_str()로 읽어오기 때문에 크기를 할당해야 한다.
	// 즉, resize가 필수이다.
	void FileStream::Read(std::string * pValue)
	{
		if (!pValue)
			return;
		
		pValue->clear();
		pValue->shrink_to_fit();

		unsigned int length = 0;
		Read(&length);

		pValue->resize(length);

		m_InputStream.read(const_cast<char*>(pValue->c_str()), length);
	}

	void FileStream::Read(std::vector<std::string>* pVec)
	{
		if (!pVec)
			return;

		pVec->clear();
		pVec->shrink_to_fit();

		unsigned int length = 0;
		Read(&length);

		pVec->resize(length);

		std::string str;
		for (unsigned int i = 0; i != length; i++)
		{
			Read(&str);
			pVec->emplace_back(str);
		}
	}

	void FileStream::Read(std::vector<unsigned char>* pVec)
	{
		if (!pVec)
			return;

		pVec->clear();
		pVec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		pVec->resize(length);

		m_InputStream.read(reinterpret_cast<char*>(pVec->data()), sizeof(unsigned char) * length);
	}

	void FileStream::Read(std::vector<std::byte>* pVec)
	{
		if (!pVec)
			return;

		pVec->clear();
		pVec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		pVec->resize(length);

		m_InputStream.read(reinterpret_cast<char*>(pVec->data()), sizeof(std::byte) * length);
	}

	void FileStream::Read(std::vector<unsigned int>* pVec)
	{
		if (!pVec)
			return;

		pVec->clear();
		pVec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		pVec->resize(length);

		m_InputStream.read(reinterpret_cast<char*>(pVec->data()), sizeof(unsigned int) * length);
	}
	/*
	void FileStream::Read(std::vector<Vertex_PosTexNorTan>* pVec)
	{
		if (!pVec)
			return;

		pVec->clear();
		pVec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		pVec->resize(length);

		m_InputStream.read(reinterpret_cast<char*>(pVec->data()), sizeof(Vertex_PosTexNorTan) * length);
	}
	*/
	void FileStream::Write(const std::string & value)
	{
		auto length = static_cast<unsigned int>(value.length());
		Write(length);

		m_OutputStream.write(value.c_str(), length);
	}

	void FileStream::Write(const std::vector<std::string>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		for (const auto& str : vec)
		{
			Write(str);
		}
	}

	void FileStream::Write(const std::vector<unsigned char>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_OutputStream.write(reinterpret_cast<const char*>(&vec[0]), sizeof(unsigned char) * size);
	}

	void FileStream::Write(const std::vector<std::byte>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_OutputStream.write(reinterpret_cast<const char*>(&vec[0]), sizeof(std::byte) * size);
	}

	void FileStream::Write(const std::vector<unsigned int>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_OutputStream.write(reinterpret_cast<const char*>(&vec[0]), sizeof(unsigned int) * size);
	}

	/*
	void FileStream::Write(const std::vector<Vertex_PosTexNorTan>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_OutputStream.write(reinterpret_cast<const char*>(&vec[0]), sizeof(Vertex_PosTexNorTan) * size);
	}
	*/
}