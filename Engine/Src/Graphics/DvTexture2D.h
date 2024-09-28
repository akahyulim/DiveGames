#pragma once
#include "DvTexture.h"

namespace Dive
{
	class DvTexture2D : public DvTexture
	{
		DV_CLASS(DvTexture2D, DvTexture)

	public:
		DvTexture2D();
		DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipChain = true);
		DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount);
		DvTexture2D(uint32_t width, uint32_t height, uint32_t depth, bool readOnly = false);
		// ����Ƽ������ ���� ��ü���� ���� �ε��ϴ� �Լ��� �������� �ʴ´�.
		// ��� ���ҽ� �Ŵ������� ������ ���޹޾� ��ü�� �����Ѵ�.
		// => �����δ� LoadImage��� �޼���� �ε�� �����͸� ���޹޾� ������ �� �ִ�.
		// => ��, �Ʒ�ó�� �����ڸ� ���� �ε��ϴ� �� �߸��� �����̴�.
		DvTexture2D(const std::string& filename, bool mipChain = true);
		DvTexture2D(const std::string& filename, uint32_t size, const void* pSource, bool mipChain = true);
		virtual ~DvTexture2D() = default;

		// ����Ƽ�� LoadImage�� �����ؾ� �Ѵ�.
		bool LoadFromFile(const std::string& filename) override;
		bool LoadFromMemory(const std::string& filename, size_t size, const void* pSource);
		bool UpdateSubresource(const void* pData, uint32_t size);

	private:
		bool createColorBuffer();
		bool createDepthBuffer(bool readOnly);
		
	private:
		DXGI_FORMAT m_Format;
	};
}