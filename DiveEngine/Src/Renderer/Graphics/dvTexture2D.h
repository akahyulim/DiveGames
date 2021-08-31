#pragma once
#include "dvTexture.h"
#include <string>
#include <DirectXMath.h>

namespace dive
{
	class dvTexture2D : public dvTexture
	{
	public:
		// 결국 어떤 텍스쳐를 생성하고, 거기엔 어떤 정보가 필요한지가 중요하다.

		dvTexture2D(const std::string& name = "")
			: dvTexture(typeid(dvTexture2D).hash_code())  
		{
			SetName(name);
		}

		bool CreateColorRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int mipCount = 1);

		bool CreateDepthRenderTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, bool readOnly = false, unsigned int mipCount = 1);

		bool LoadFromFile(const std::string& filepath, bool createMips = false);
		bool SaveToFile(const std::string& filepath);

		void Clear();


		// static 함수로 특정 색상의 텍스쳐를 생성 = 유니티에 있었던 것 같다.

	private:

	private:
	};
}