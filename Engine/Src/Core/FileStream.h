#pragma once

namespace Dive
{
	class GameObject;

	enum class eFileStreamMode
	{
		Read,
		Write,
		Append,
	};

	class FileStream
	{
	public:
		FileStream(const std::string& filepath, eFileStreamMode mode);
		~FileStream();

		bool IsOpen() const { return m_bOpened; }
		void Close();

		// 쓰기

		// 읽기

	private:
	private:
		std::ofstream m_out;
		std::ifstream m_in;
		eFileStreamMode m_mode;
		bool m_bOpened;
	};
}