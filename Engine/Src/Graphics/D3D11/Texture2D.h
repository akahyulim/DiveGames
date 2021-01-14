#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	// 일단 Array, Mipmap은 전부 제외
	class Texture2D : public Texture
	{
	public:
		Texture2D(Context* context, std::string name);
		~Texture2D();

		// 이렇게 하면 Base인 Texture로 사용이 불가능하다.
		// 그런데 굳이 2D와 Cube를 Texture Interface로 묶어 사용할 필요가 있나 싶다.
		// load image file = shaderResourceView
		bool CreateRenderTarget(unsigned int width, unsigned int height, DXGI_FORMAT format);
		bool CreateDepthStencil(unsigned int width, unsigned int height, bool useStencil);

		ID3D11Resource* GetResource() { return static_cast<ID3D11Resource*>(m_resource); }

	private:

	private:
		ID3D11Texture2D* m_resource;
	};
}