/*
	
	¡¶¿€: º≠∫∏¿±( akahyulim@gmail.com )
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

		std::wstring GetCurrentW();
	}
}