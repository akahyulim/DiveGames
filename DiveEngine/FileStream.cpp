#include "FileStream.h"
#include "Log.h"


namespace Dive
{
	FileStream::FileStream(const std::string & filepath, eFileStreamMode mode)
		: m_bOpened(false),
		m_mode(mode)
	{
		int iosFlags = std::ios::binary;
		iosFlags |= m_mode == eFileStreamMode::Read ? std::ios::in : 0;
		iosFlags |= m_mode == eFileStreamMode::Write ? std::ios::out : 0;
		iosFlags |= m_mode == eFileStreamMode::Append ? std::ios::app : 0;

		if (m_mode == eFileStreamMode::Write || m_mode == eFileStreamMode::Append)
		{
			m_out.open(filepath, iosFlags);
			if (m_out.fail())
			{
				CORE_ERROR("");
				return;
			}
		}
		else if (m_mode == eFileStreamMode::Read)
		{
			m_in.open(filepath, iosFlags);
			if (m_in.fail())
			{
				CORE_ERROR("");
				return;
			}
		}

		m_bOpened = true;
	}

	FileStream::~FileStream()
	{
		Close();
	}

	void FileStream::Close()
	{
		if (m_mode == eFileStreamMode::Write || m_mode == eFileStreamMode::Append)
		{
			m_out.flush();
			m_out.close();
		}
		else if (m_mode == eFileStreamMode::Read)
		{
			m_in.clear();
			m_in.close();
		}
	}

	// 1. capacity가 0으로 초기화되지 않고 있다.
	// 2. value->c_str()로 읽어오기 때문에 크기를 할당해야 한다.
	// 즉, resize가 필수이다.
	void FileStream::Read(std::string * value)
	{
		if (!value)
			return;
		
		value->clear();
		value->shrink_to_fit();

		unsigned int length = 0;
		Read(&length);

		value->resize(length);

		m_in.read(const_cast<char*>(value->c_str()), length);
	}

	void FileStream::Read(std::vector<std::string>* vec)
	{
		if (!vec)
			return;

		vec->clear();
		vec->shrink_to_fit();

		unsigned int length = 0;
		Read(&length);

		vec->resize(length);

		std::string str;
		for (unsigned int i = 0; i != length; i++)
		{
			Read(&str);
			vec->emplace_back(str);
		}
	}

	void FileStream::Read(std::vector<unsigned char>* vec)
	{
		if (!vec)
			return;

		vec->clear();
		vec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		vec->resize(length);

		m_in.read(reinterpret_cast<char*>(vec->data()), sizeof(unsigned char) * length);
	}

	void FileStream::Read(std::vector<std::byte>* vec)
	{
		if (!vec)
			return;

		vec->clear();
		vec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		vec->resize(length);

		m_in.read(reinterpret_cast<char*>(vec->data()), sizeof(std::byte) * length);
	}

	void FileStream::Read(std::vector<unsigned int>* vec)
	{
		if (!vec)
			return;

		vec->clear();
		vec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		vec->resize(length);

		m_in.read(reinterpret_cast<char*>(vec->data()), sizeof(unsigned int) * length);
	}
	/*
	void FileStream::Read(std::vector<Vertex_PosTexNorTan>* vec)
	{
		if (!vec)
			return;

		vec->clear();
		vec->shrink_to_fit();

		auto length = ReadAs<unsigned int>();

		vec->resize(length);

		m_in.read(reinterpret_cast<char*>(vec->data()), sizeof(Vertex_PosTexNorTan) * length);
	}
	*/
	void FileStream::Write(const std::string & value)
	{
		auto length = static_cast<unsigned int>(value.length());
		Write(length);

		m_out.write(value.c_str(), length);
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

		m_out.write(reinterpret_cast<const char*>(&vec[0]), sizeof(unsigned char) * size);
	}

	void FileStream::Write(const std::vector<std::byte>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_out.write(reinterpret_cast<const char*>(&vec[0]), sizeof(std::byte) * size);
	}

	void FileStream::Write(const std::vector<unsigned int>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_out.write(reinterpret_cast<const char*>(&vec[0]), sizeof(unsigned int) * size);
	}

	/*
	void FileStream::Write(const std::vector<Vertex_PosTexNorTan>& vec)
	{
		auto size = static_cast<unsigned int>(vec.size());
		Write(size);

		m_out.write(reinterpret_cast<const char*>(&vec[0]), sizeof(Vertex_PosTexNorTan) * size);
	}
	*/
}