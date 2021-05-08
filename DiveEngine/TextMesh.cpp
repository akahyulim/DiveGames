#include "TextMesh.h"

namespace Dive
{
	TextMesh::TextMesh()
	{
		// 멤버 변수 초기화

		// Resource Manager 등으로부터 Default Font를 얻어온다.
		// Font자체는 Renderer 등에서 Face별로 생성된다.
		// 그리고 해당 Font로부터 Size 등으로 저장한다.
	}

	TextMesh::~TextMesh()
	{
	}
	
	bool TextMesh::SetFont(const Dive_Font* pFont)
	{
		assert(pFont != nullptr);

		if (m_pFont != pFont)
			m_pFont = const_cast<Dive_Font*>(pFont);

		return true;
	}

	// 결국 ascii와 unicode로 나뉘어야 한다.
	bool TextMesh::SetText(const std::string& text)
	{
		// 이게 좀 복잡하다.
		// 일단 문장 자체는 저장해야 한다. 그래야 이전 입력을 다시 그리지 않을 수 있다.
		// 그런데 이후 Font의 Face나 Size가 변경되면
		// 이들을 다시 텍스쳐에 세겨야 한다.
		// 그리고 이에 맞춰 버퍼도 다시 생성해줘야 한다.
		// 따라서 이 곳에서 버퍼를 생성하는게 맞는지 생각해볼 필요가 있다.

		return true;
	}

	void TextMesh::SetFontSize(unsigned int size)
	{
		if (m_fontSize == size)
			return;

		// Font를 다시 구성해야 한다.

		m_fontSize = size;
	}
}