#pragma once
#include "Texture.h"

namespace Dive
{
	class TextureCube : public Texture
	{
	public:
		RESOURCE_CLASS_TYPE(TextureCube)
	public:
		TextureCube();
		TextureCube(unsigned long long id);

		bool create() override;

		bool SetSize(int size, DXGI_FORMAT format, unsigned int bindFlags);

	private:
	private:
	};
}