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

		// 이건 기본적으로 stv용이다.
		bool SetSize(int size, DXGI_FORMAT format, unsigned int bindFlags);

		// 스카이 박스를 만드려면 결국 6장의 텍스쳐를 임포트 해야 한다.
		// 그리고 스파르탄의 사용법을 보면 srv를 shader에서 처리하는 듯 하다.
		bool LoadTexture(
			const std::string& front,
			const std::string& back,
			const std::string& up,
			const std::string& down,
			const std::string& left,
			const std::string& right,
			unsigned int bindFlags,
			bool generateMips
		);

	private:
	private:
	};
}