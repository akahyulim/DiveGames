#pragma once
#include "Texture.h"

namespace Dive
{
	class Context;

	class Texture2D : public Texture
	{
		DIVE_OBJECT(Texture2D, Texture)

	public:
		explicit Texture2D(Context* pContext);
		~Texture2D() override;

		static void RegisterObject(Context* pContext);

		bool LoadFromFile(const std::string& fileName) override;
		void Release();

		bool SetSize(int width, int height, DXGI_FORMAT format, eTextureUsage usage = eTextureUsage::Static);

		bool SetData(const void* pData);

	private:

	private:
	};
}