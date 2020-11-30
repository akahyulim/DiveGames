#pragma once
#include "Texture.h"

// 1. createTextureResource()안에서 다시 함수를 호출하는건 에바다.
// 2. 생성 인자에 대한 이해도를 높인 후 다듬을 필요가 있다.
// 3. 구현을 마치면 Editor에 띄워보는 테스트를 수행하자.

namespace Dive
{
	class SystemManager;

	class Texture2D : public Texture
	{
	public:
		// sharder resource?
		Texture2D(SystemManager * manager, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data);

		// render target
		Texture2D(SystemManager * manager, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name = " ");

	private:
		bool createTextureResource() override;
	};
}