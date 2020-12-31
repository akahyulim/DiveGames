/*
	
	제작: 서보윤( akahyulim@gmail.com )
*/
#pragma once

namespace Dive
{
	namespace File
	{
		bool ExistsA(const std::string& name);
		bool DeleteA(const std::string& name);

		bool ExistsW(const std::wstring& name);
		bool DeleteW(const std::wstring& name);
	}

	namespace Dir
	{
		std::string GetCurrentA();
		// file path로부터 filename + format 얻기
		// file path로부터 filename만 얻기

		std::wstring GetCurrentW();
	}

	namespace EngineFormat
	{
		// file path로부터 지원하는 포멧인지 확인
		// 각종 리소스가 해당(scene, texture, model, etc)
	}
}