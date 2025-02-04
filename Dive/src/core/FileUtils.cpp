#include "stdafx.h"
#include "FileUtils.h"
#include "Log.h"
#include <commdlg.h>
#include <ShlObj.h>
#include <combaseapi.h>

namespace Dive
{
	std::filesystem::path FileUtils::OpenFile(const char* pFilter, HWND owner, LPCSTR pDir)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		CHAR currentDir[256] = { 0 };
		GetCurrentDirectoryA(256, currentDir);
		ofn.lpstrInitialDir = pDir ? pDir : currentDir;
		ofn.lpstrFilter = pFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return std::filesystem::path(ofn.lpstrFile);

		return std::filesystem::path();
	}

	std::filesystem::path FileUtils::SaveAs(const std::filesystem::path& oldFilePath, const char* pFilter, HWND owner)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		//CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		//if (GetCurrentDirectoryA(256, currentDir))
		ofn.lpstrInitialDir = oldFilePath.parent_path().string().c_str();
		ofn.lpstrFilter = pFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = strchr(pFilter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			std::filesystem::path newFilePath(ofn.lpstrFile);
			try
			{
				std::filesystem::copy(oldFilePath, newFilePath, std::filesystem::copy_options::overwrite_existing);
				return newFilePath;
			}
			catch (const std::filesystem::filesystem_error& e)
			{
				DV_LOG(FileUtils, err, "Save As에 실패하였습니다: {:s}", e.what());
			}
		}

		return std::filesystem::path();
	}

	std::string FileUtils::SelectFolder() 
	{ 
		// Initialize COM library 
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); 
		if (FAILED(hr)) 
		{ 
			return std::string(); 
		} 
		std::string folderPath; 
		IFileDialog* pfd = nullptr; 
		
		// Create the File Open Dialog object 
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)); 
		if (SUCCEEDED(hr)) 
		{ 
			DWORD dwOptions; 
			
			// Set the options on the dialog 
			if (SUCCEEDED(pfd->GetOptions(&dwOptions))) 
			{ 
				pfd->SetOptions(dwOptions | FOS_PICKFOLDERS); 
			} 
			
			// Show the dialog 
			hr = pfd->Show(NULL); 
			if (SUCCEEDED(hr)) 
			{ 
				// Get the result 
				IShellItem* psiResult; 
				hr = pfd->GetResult(&psiResult); 
				if (SUCCEEDED(hr)) 
				{ 
					// Get the display name of the folder 
					PWSTR pszPath = NULL; 
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath); 
					if (SUCCEEDED(hr)) 
					{ 
						// Convert the display name to a std::string 
						char path[MAX_PATH]; 
						WideCharToMultiByte(CP_ACP, 0, pszPath, -1, path, MAX_PATH, NULL, NULL); 
						folderPath = path; 
						CoTaskMemFree(pszPath); 
					} 
					psiResult->Release(); 
				} 
			}

			pfd->Release(); 
		} 
		
		// Uninitialize COM library 
		CoUninitialize(); 
		
		return folderPath; 
	}
}