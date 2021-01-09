#pragma once
#include "Texture.h"

// 1. createTextureResource()안에서 다시 함수를 호출하는건 에바다.
// 2. 생성 인자에 대한 이해도를 높인 후 다듬을 필요가 있다.


namespace Dive
{
	class Context;

	// 스파르탄은 Texture에 viewport가 멤버변수로 존재하며,
	// 모든 생성함수에서 크기에 맞춰 초기화한다.
	class Texture2D : public Texture
	{
	public:
		// sharder resource?
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data);

		
		// format을 기준으로 RenderTargetView & ShaderResourceView 혹은 DepthStencilView & ShaderResourceView로 생성한다.	
		Texture2D(Context* context, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name = " ");

	private:
		bool createTextureResource() override;
	};
}