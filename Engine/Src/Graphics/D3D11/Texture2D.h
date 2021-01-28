#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	// �ϴ� Array, Mipmap�� ���� ����
	// ��� �����ڰ� Resource�� RenderTargetView ���� �Լ��� ������.
	// ���� private �Լ��� ������.
	class Texture2D : public Texture
	{
	public:
		// load image file = shaderResourceView
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, std::string name = "");
		Texture2D(Context* context, unsigned int width, unsigned int height, bool useStencil = true, std::string name = "");
		~Texture2D();

		ID3D11Resource* GetResource() { return static_cast<ID3D11Resource*>(m_resource); }

	private:

	private:
		ID3D11Texture2D* m_resource;
	};
}