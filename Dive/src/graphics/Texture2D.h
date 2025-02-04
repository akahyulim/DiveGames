#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
		DV_CLASS(Texture2D, Texture)

	public:
		Texture2D();
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipChain = true);
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount);
		Texture2D(uint32_t width, uint32_t height, uint32_t depth, bool useReadOnly = false);
		// ����Ƽ������ ���� ��ü���� ���� �ε��ϴ� �Լ��� �������� �ʴ´�.
		// ��� ���ҽ� �Ŵ������� ������ ���޹޾� ��ü�� �����Ѵ�.
		// => �����δ� LoadImage��� �޼���� �ε�� �����͸� ���޹޾� ������ �� �ִ�.
		// => ��, �Ʒ�ó�� �����ڸ� ���� �ε��ϴ� �� �߸��� �����̴�.
		Texture2D(const std::filesystem::path& path, bool mipChain = true);
		Texture2D(const std::filesystem::path& path, uint32_t size, const void* pSource, bool mipChain = true);
		virtual ~Texture2D() = default;

		// ����Ƽ�� LoadImage�� �����ؾ� �Ѵ�.
		bool LoadFromFile(const std::filesystem::path& path) override;
		bool LoadFromMemory(const std::filesystem::path& path, size_t size, const void* pSource);
		bool UpdateSubresource(const void* pData, uint32_t size);

	private:
		bool createColorBuffer();
		bool createDepthBuffer(bool readOnly);
	};
}