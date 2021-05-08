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
		TextMesh();
		~TextMesh();

		bool SetFont(const Dive_Font* pFont);

		bool SetText(const std::string& text);

		// 만약 크기 변경 함수를 호출한다면
		// Font에서 다시 텍스쳐를 만들어야 한다.
		// 그리고 이 경우 기존 출력까지도 변경된 크기를 적용받아야 한다.
		// 일반적으로 이러한 논리는 당연하다.
		void SetFontSize(unsigned int size);

		ID3D11Buffer* GetVertexBuffer();
		ID3D11Buffer* GetIndexBuffer();
		// 랩핑함수 호출일 수 밖에 없다.
		ID3D11ShaderResourceView* GetAtlas();

	private:

	private:
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIdexBuffer = nullptr;

		// Font는 결국 Resource가 되어야 한다.
		Dive_Font* m_pFont = nullptr;

		// state를 직접 만들어야 하나? 생성된 걸 가져야 하나?

		unsigned int m_fontSize;
	};
}