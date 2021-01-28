#pragma once
#include "Texture.h"


namespace Dive
{
	class Context;

	// 일단 Array, Mipmap은 전부 제외
	// 모든 생성자가 Resource와 RenderTargetView 생성 함수를 가진다.
	// 추후 private 함수로 나누자.
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