#pragma once

namespace Dive
{
	class Texture;

	class ImageImporter
	{
	public:
		ImageImporter();
		~ImageImporter();

		bool Load(const std::string& filepath, Texture* pTexture);
		bool LoadArray(std::vector<std::string> filepaths, Texture* pTexture);

	private:
	private:
		ID3D11Device* m_pDevice = nullptr;	// ���� ����� �� �𸣰ڴ�.
	};
}