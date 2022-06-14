#pragma once

namespace Dive
{
	class Image
	{
	public:
		Image() = default;
		~Image();

		// urho의 경우 resource 함수인 듯 하다. override하지도 않았다.
		// 즉, Reset, Clear, Release를 전부 다르게 사용한다.
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