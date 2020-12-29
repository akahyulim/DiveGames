#include "DivePch.h"
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
}