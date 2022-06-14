#pragma once

namespace Dive
{
	class Image
	{
	public:
		Image() = default;
		~Image();

		// urho�� ��� resource �Լ��� �� �ϴ�. override������ �ʾҴ�.
		// ��, Reset, Clear, Release�� ���� �ٸ��� ����Ѵ�.
		void Reset();

		bool Load(const std::string& filepath);

	private:
	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		uint32_t m_ArraySize = 1;
		uint32_t m_MipLevels = 1;
		bool m_bCubeMap = false;
		bool m_bUseAlpha = false;
		
		unsigned char* m_pData = nullptr;
	};
}