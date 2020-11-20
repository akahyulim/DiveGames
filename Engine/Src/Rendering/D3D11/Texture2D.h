#pragma once
#include "Texture.h"


namespace Dive
{
	class SystemManager;

	class Texture2D : public Texture
	{
	public:
		// sharder resource?
		Texture2D(SystemManager* manager, unsigned int width, unsigned int height, DXGI_FORMAT format, const std::vector<std::vector<std::byte>>& data);

		// render target
		Texture2D(SystemManager* manager, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int arraySize, unsigned int flags, std::string name = " ");

	private:
		bool createTextureResource() override;
	};
}