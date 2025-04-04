#include "stdafx.h"
#include "FileUtils.h"
#include "Log.h"
#include <commdlg.h>
#include <ShlObj.h>
#include <combaseapi.h>

namespace Dive
{
    std::filesystem::path FileUtils::OpenFile(const char* filter, HWND owner, LPCSTR dir) 
    {
        std::vector<char> file(MAX_PATH, '\0');

        OPENFILENAMEA ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = owner;
        ofn.lpstrFile = file.data();
        ofn.nMaxFile = static_cast<DWORD>(MAX_PATH);

        std::vector<char> currentDir(MAX_PATH, '\0');
        GetCurrentDirectoryA(static_cast<DWORD>(currentDir.size()), currentDir.data());
        ofn.lpstrInitialDir = dir ? dir : currentDir.data();

        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn))
            return std::filesystem::path(ofn.lpstrFile);

        return {};
    }

    std::filesystem::path FileUtils::SaveAs(const std::filesystem::path& oldFilePath, const char* filter, HWND owner) 
    {
        std::vector<char> file(MAX_PATH, '\0');

        OPENFILENAMEA ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = owner;
        ofn.lpstrFile = file.data();
        ofn.nMaxFile = static_cast<DWORD>(MAX_PATH);
        ofn.lpstrInitialDir = oldFilePath.parent_path().string().c_str();
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        const char* defaultExt = strchr(filter, '\0') + 1;
        ofn.lpstrDefExt = defaultExt;

        if (GetSaveFileNameA(&ofn))
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

        return {};
    }

	std::string FileUtils::SelectFolder() 
	{ 
		HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(result))
			return {};

		std::string folderPath;

		using Microsoft::WRL::ComPtr;
		ComPtr<IFileDialog> fileDialog;
		result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog));
		if (SUCCEEDED(result))
        {
			DWORD options;
			if (SUCCEEDED(fileDialog->GetOptions(&options)))
				fileDialog->SetOptions(options | FOS_PICKFOLDERS);

			if (SUCCEEDED(fileDialog->Show(nullptr)))
            {
				ComPtr<IShellItem> shellItem;
				if (SUCCEEDED(fileDialog->GetResult(&shellItem)))
                {
					PWSTR path = nullptr;
					if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                    {
						std::vector<char> temp(MAX_PATH);
						WideCharToMultiByte(CP_ACP, 0, path, -1, temp.data(), static_cast<int>(temp.size()), nullptr, nullptr);
						folderPath = temp.data();
						CoTaskMemFree(path);
					}
				}
			}
		}

		CoUninitialize();
		return folderPath;
	}
}
