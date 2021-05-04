#pragma once
#include "Renderer.h"
#include "Dive_Font.h"
#include <string>

namespace Dive
{
	// ���� Mesh�� ������???
	class TextMesh
	{
	public:

		bool SetText(const std::string& text);

		ID3D11Buffer* GetVertexBuffer();
		ID3D11Buffer* GetIndexBuffer();
		// �����Լ� ȣ���� �� �ۿ� ����.
		ID3D11ShaderResourceView* GetAtlas();
		ID3D11ShaderResourceView* GetAtlasOutline();

	private:
	private:
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIdexBuffer = nullptr;

		// Font�� �ᱹ Resource�� �Ǿ�� �Ѵ�.
		Dive_Font* m_pFont = nullptr;

		// state�� ���� ������ �ϳ�? ������ �� ������ �ϳ�?
	};
}