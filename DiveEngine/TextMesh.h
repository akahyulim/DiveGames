#pragma once
#include "Renderer.h"
#include "Dive_Font.h"
#include <string>

namespace Dive
{
	// 추후 Mesh를 상속토록???
	class TextMesh
	{
	public:

		bool SetText(const std::string& text);

		ID3D11Buffer* GetVertexBuffer();
		ID3D11Buffer* GetIndexBuffer();
		// 랩핑함수 호출일 수 밖에 없다.
		ID3D11ShaderResourceView* GetAtlas();
		ID3D11ShaderResourceView* GetAtlasOutline();

	private:
	private:
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIdexBuffer = nullptr;

		// Font는 결국 Resource가 되어야 한다.
		Dive_Font* m_pFont = nullptr;

		// state를 직접 만들어야 하나? 생성된 걸 가져야 하나?
	};
}