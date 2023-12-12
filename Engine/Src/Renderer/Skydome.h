#pragma once

namespace Dive
{
	class ShaderVariation;
	class StaticMesh;
	class Texture2D;
	class ConstantBuffer;

	struct SkydomeConstantBuffer
	{
		DirectX::XMFLOAT4 apexColor;
		DirectX::XMFLOAT4 centerColor;
	};

	// 유니티의 경우 머티리얼이다.
	// skybox라는 머티리얼이 존재하고					=> 머티리얼을 생성한 후 스카이박스를 선택하고 나서 다시 구체 쉐이더를 선택한다.
	// 다시 6sided, cubemap, procedual로 나뉜다.		=> 어떤 글에서는 각각 쉐이더라 칭한다.
	// 6sided는 텍스쳐 6개로 스카이박스를 구성하고
	// 큐브맵은 큐브맵 텍스쳐로 스카이박스를 구현한다.
	// 마지막으로 프로시저는 태양의 크기, 대기 색, 바닥 색 등을 설정하는데 현재 스카이돔의 확장이라 할 수 있다.
	// 참고 영상에서는 이 머티리얼을 뷰포트에 옮기면 바로 적용되는데
	// 아마도 RenderSettings에 등록되는 듯 하다.		=> RenderSettings 클래스가 존재하는데 메뉴는 LightingSettings의 Scene를 사용한다.
	// 카메라는 배경을 스카이박스로 할 것인지 단일색(백그라운드)으로 할 것인지 정할 뿐이다.
	class Skydome
	{
	public:
		Skydome();
		~Skydome();

		void Update();
		void Render();

		DirectX::XMFLOAT4 GetApexColor() const { return m_ApexColor; }
		void SetApexColor(DirectX::XMFLOAT4 color) { m_ApexColor = color; }

		DirectX::XMFLOAT4 GetCenterColor() const { return m_CenterColor; }
		void SetCenterColor(DirectX::XMFLOAT4 color) { m_CenterColor = color; }

		Texture2D* GetTexture() const { return m_pTexture; }
		void SetTexture(Texture2D* pTex) { m_pTexture = pTex; }


	private:
	private:
		StaticMesh* m_pMesh;
		DirectX::XMFLOAT4 m_ApexColor;
		DirectX::XMFLOAT4 m_CenterColor;
		Texture2D* m_pTexture;
		ConstantBuffer* m_pConstantBuffer;
		ShaderVariation* m_pVertexShader;
		ShaderVariation* m_pPixelShader;
	};
}