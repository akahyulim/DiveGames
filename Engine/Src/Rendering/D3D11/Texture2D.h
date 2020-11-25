#pragma once
#include "Texture.h"

// 1. createTextureResource()�ȿ��� �ٽ� �Լ��� ȣ���ϴ°� ���ٴ�.
// 2. ���� ���ڿ� ���� ���ص��� ���� �� �ٵ��� �ʿ䰡 �ִ�.
// 3. ������ ��ġ�� Editor�� ������� �׽�Ʈ�� ��������.

namespace Dive
{
	class SystemManager;

	class Texture2D : public Texture
	{
	public:
		// sharder resource?
		Texture2D(ID3D11Device* device, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data);

		// render target
		Texture2D(ID3D11Device* device, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name = " ");

	private:
		bool createTextureResource() override;
		bool createTexture(ID3D11Texture2D** texture);
		bool createRenderTargetView(ID3D11Texture2D* texture);
		bool createDepthStencilView(ID3D11Texture2D* texture);
		bool createShaderResourceView(ID3D11Texture2D* texture);
	};
}