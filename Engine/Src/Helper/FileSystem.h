#pragma once
#include <filesystem>

namespace Dive
{
	namespace Helper
	{
		class FileSystem
		{
		public:
			static std::string GetDirectory(const std::string& filepath);
			static std::string GetFilepathWithoutExtension(const std::string& filepath);
			static std::string GetFileName(const std::string& filepath);
			static std::string GetFileNameWithoutExtension(const std::string& filepath);

			static std::wstring StringToWString(const std::string& str);
		};

        static const std::vector<std::string> supported_formats_image
        {
                ".jpg",
                ".png",
                ".bmp",
                ".tga",
                ".dds",
                ".exr",
                ".raw",
                ".gif",
                ".hdr",
                ".ico",
                ".iff",
                ".jng",
                ".jpeg",
                ".koala",
                ".kodak",
                ".mng",
                ".pcx",
                ".pbm",
                ".pgm",
                ".ppm",
                ".pfm",
                ".pict",
                ".psd",
                ".raw",
                ".sgi",
                ".targa",
                ".tiff",
                ".tif", // tiff can also be tif
                ".wbmp",
                ".webp",
                ".xbm",
                ".xpm"
        };
	}
}
