#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	// �ϴ� Array, Mipmap�� ���� ����
	class Texture2D : public Texture
	{
	public:
		Texture2D(Context* context, std::string name);
		~Texture2D();

		// �̷��� �ϸ� Base�� Texture�� ����� �Ұ����ϴ�.
		// �׷��� ���� 2D�� Cube�� Texture Interface�� ���� ����� �ʿ䰡 �ֳ� �ʹ�.
		// load image file = shaderResourceView
		bool CreateRenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format);
		bool CreateDepthStencil(unsigned int width, unsigned int height, bool useStencil);

		ID3D11Resource* GetResource() { return static_cast<ID3D11Resource*>(m_resource); }

	private:

	private:
		ID3D11Texture2D* m_resource;
	};
}