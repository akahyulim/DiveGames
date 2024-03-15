#pragma once
#include "Resource.h"
#include <DirectXTex/DirectXTex.h>

namespace Dive
{
	// ���� Texture�� Image�� ������ �ʿ䰡 ������?
	// �׸��� Model, Mesh�� ����ó�� ���ҽ��� �ʿ䰡 ������?
	class Image : public Resource
	{
		DV_CLASS(Image, Resource);

	public:
		Image();
		~Image() override = default;

		bool LoadFromFile(const std::string& fileName) override;
		bool LoadFromMemory(const std::string& extension, size_t size, const void* pSrcData);

		bool SetSize(int width, int height, DXGI_FORMAT format);
		bool SetSize(int widht, int height, int depth, DXGI_FORMAT format);

		// �̸��� RawData��...?
		void SetPixels(const uint8_t* pPixels);

		void SetPixel(int width, int height, const DirectX::XMFLOAT4& color);
		void SetPixel(int width, int height, int depth, const DirectX::XMFLOAT4& color);

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		int GetDepth() const { return m_Depth; }

		DXGI_FORMAT GetFormat() const { return m_Format; }

		// ���� �̸��� �򰥸��µ�... �����ҰŶ�� ��� ������ �Բ�...
		const uint8_t* GetPixels() const { return m_Pixels.data(); }

		DirectX::XMFLOAT4 GetPixel(int width, int height);
		DirectX::XMFLOAT4 GetPixel(int width, int height, int depth);

	private:
		void loadScrachImage(const DirectX::ScratchImage& img);

	private:
		int m_Width;
		int m_Height;
		int m_Depth;

		DXGI_FORMAT m_Format;

		bool m_bCubemap;

		std::vector<uint8_t> m_Pixels;
	};
}