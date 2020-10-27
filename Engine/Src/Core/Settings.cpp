#include "DivePch.h"
#include "Settings.h"
#include "FileSystemHelper.h"

namespace SettingsIO
{
	std::ofstream fout;
	std::ifstream fin;
	std::string fileName = "DiveOptions.ini";
}

namespace Dive
{
	template <class T>
	void WriteSetting(std::ofstream& fout, const std::string& name, T value)
	{
		fout << name << "=" << value << std::endl;
	}

	template <class T>
	void ReadSetting(std::ifstream& fin, const std::string& name, T& value)
	{
		for (std::string line; getline(fin, line); )
		{
			auto firstIndex = line.find_first_of('=');
			if (name == line.substr(0, firstIndex))
			{
				auto lastindex = line.find_last_of('=');
				std::string readValue = line.substr(lastindex + 1, line.length());
				value = (T)stof(readValue);
				return;
			}
		}
	}

	void Settings::Initialize()
	{
		/*
		if(FileSystemHelper::FileExists(SettingsIO::fileName))
		{
			// 파일이 있다면 로드
			SettingsIO::fin.open(SettingsIO::fileName, std::ifstream::in);

			ReadSetting(SettingsIO::fin, "ScreenMode", m_ScreenMode);
			ReadSetting(SettingsIO::fin, "ResolutionWidth", m_ResolutionWidth);
			ReadSetting(SettingsIO::fin, "ResolutionHeight", m_ResolutionHeight);
			ReadSetting(SettingsIO::fin, "FrameRate", m_FrameRate);
			ReadSetting(SettingsIO::fin, "VSync", m_VSync);

			SettingsIO::fin.close();
		}
		else
		{
			// 파일이 없다면 초기화 후 파일 저장
			SettingsIO::fout.open(SettingsIO::fileName, std::ofstream::out);

			WriteSetting(SettingsIO::fout, "ScreenMode", m_ScreenMode);
			WriteSetting(SettingsIO::fout, "ResolutionWidth", m_ResolutionWidth);
			WriteSetting(SettingsIO::fout, "ResolutionHeight", m_ResolutionHeight);
			WriteSetting(SettingsIO::fout, "FrameRate", m_FrameRate);
			WriteSetting(SettingsIO::fout, "VSync", m_VSync);
			
			SettingsIO::fout.close();
		}
		*/
	}
}
